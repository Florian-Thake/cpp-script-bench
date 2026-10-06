/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */

// The AngelScript runner, with the add-ons for strings, arrays, dictionaries and math. This file is built twice:
//   csb_angelscript            AngelScript in its default configuration, which is thread-safe.
//   csb_angelscript_nothreads  with AS_NO_THREADS: no thread safety for multi-threaded use.
// Each build has two variants: scripts compiled with line cues (the default), and without them
// (asEP_BUILD_WITHOUT_LINE_CUES). The AngelScript manual recommends both AS_NO_THREADS and building
// without line cues for performance when they are not needed (section "Fine tuning").

#include "registry.hpp"

#include <csb/runner.hpp>

#include <angelscript.h>
#include <scriptarray/scriptarray.h>
#include <scriptdictionary/scriptdictionary.h>
#include <scriptmath/scriptmath.h>
#include <scriptstdstring/scriptstdstring.h>

#include <chrono>
#include <climits>
#include <stdexcept>


namespace {

struct Variant
{
    std::string_view  name;
    bool              line_cues;  ///< false: scripts are built with asEP_BUILD_WITHOUT_LINE_CUES
};

// The no-threads build without line cues is the headline: AngelScript's documented fastest configuration, and
// like ChaiScript's headline (no-threads) the closer match to the single-threaded engines of the others.
#if defined( AS_NO_THREADS )
constexpr Variant variants[] = {
    { "no-threads",              true },
    { "no-threads-no-line-cues", false },
};
constexpr char const default_variant[] = "no-threads-no-line-cues";
std::optional<std::string> const headline_variant = "no-threads-no-line-cues";
#else
constexpr Variant variants[] = {
    { "default",      true },
    { "no-line-cues", false },
};
constexpr char const default_variant[] = "default";
std::optional<std::string> const headline_variant = std::nullopt;
#endif


using Clock = std::chrono::steady_clock;

double seconds_since( Clock::time_point const start )
{
    return std::chrono::duration<double>( Clock::now() - start ).count();
}

/// Throws if \param r is an AngelScript error code (negative).
void check( int const r, char const *const what )
{
    if( r < 0 ) {
        throw std::runtime_error( std::string( what ) + " failed with AngelScript error code " + std::to_string( r ) );
    }
}

/// Collects the messages of the compiler, e.g. syntax errors.
void message_callback( asSMessageInfo const *const msg, void *const param )
{
    auto &messages = *static_cast<std::string *>(param);
    char const *const type = msg->type == asMSGTYPE_ERROR ? "error" : msg->type == asMSGTYPE_WARNING ? "warning" : "info";
    messages += std::string( msg->section ) + " (" + std::to_string( msg->row ) + ", " + std::to_string( msg->col ) + ") "
              + type + ": " + msg->message + "\n";
}

/// The compile options of the library, e.g. "AS_NO_THREADS AS_WIN AS_X64_MSVC".
std::string library_options()
{
    std::string_view options = asGetLibraryOptions();
    while( not options.empty() && options.front() == ' ' ) {
        options.remove_prefix( 1 );
    }
    while( not options.empty() && options.back() == ' ' ) {
        options.remove_suffix( 1 );
    }
    return std::string( options );
}


class AngelScriptLanguage final : public csb::Language
{
    Variant              mVariant;
    asIScriptEngine     *mEngine  = nullptr;
    asIScriptContext    *mContext = nullptr;  // reused for all calls, as the AngelScript manual recommends
    asIScriptFunction   *mBench   = nullptr;  // looked up once after loading
    std::string          mMessages;           // compiler messages of the last build

public:
    explicit AngelScriptLanguage( Variant const &rVariant )
        : mVariant( rVariant )
    {
    }

    std::string name() const override { return "AngelScript"; }

    std::string version() const override
    {
        return std::string( ANGELSCRIPT_VERSION_STRING ) + " (" + library_options() + ")";
    }

    std::string script_extension() const override { return ".as"; }

    void create_engine() override
    {
        mEngine = asCreateScriptEngine();
        if( mEngine == nullptr ) {
            throw std::runtime_error( "asCreateScriptEngine() failed" );
        }
        check( mEngine->SetMessageCallback( asFUNCTION( message_callback ), &mMessages, asCALL_CDECL ), "SetMessageCallback()" );
        check( mEngine->SetEngineProperty( asEP_BUILD_WITHOUT_LINE_CUES, mVariant.line_cues ? 0 : 1 ), "SetEngineProperty()" );

        // The add-ons which form AngelScript's usual standard library, like the other languages get theirs.
        // There is no file or module loading (the scriptfile and scriptbuilder add-ons are not used), so there is no disk IO.
        RegisterStdString( mEngine );
        RegisterScriptArray( mEngine, true );  // true: 'int[]' is the syntax for arrays
        RegisterStdStringUtils( mEngine );
        RegisterScriptDictionary( mEngine );
        RegisterScriptMath( mEngine );

        mContext = mEngine->CreateContext();
        if( mContext == nullptr ) {
            throw std::runtime_error( "CreateContext() failed" );
        }
    }

    std::vector<csb::Phase> load( csb::BenchmarkInfo const &info, std::string const &source ) override
    {
        std::vector<csb::Phase> phases;
        std::string const section_name = std::string( info.name ) + script_extension();
        mMessages.clear();

        // Build() parses and compiles in one step and initializes the global variables.
        // AngelScript has no top-level statements, so there is no separate top-level phase.
        auto const start = Clock::now();
        asIScriptModule *const mod = mEngine->GetModule( "bench", asGM_ALWAYS_CREATE );
        check( mod->AddScriptSection( section_name.c_str(), source.data(), source.size() ), "AddScriptSection()" );
        if( mod->Build() < 0 ) {
            throw std::runtime_error( "AngelScript build failed:\n" + mMessages );
        }
        phases.push_back( { "parse_compile", seconds_since( start ) } );

        mBench = mod->GetFunctionByDecl( "int bench( int )" );
        if( mBench == nullptr ) {
            throw std::runtime_error( "the script does not define the function int bench( int n )" );
        }
        return phases;
    }

    std::int64_t call_bench( std::int64_t const n ) override
    {
        // int is AngelScript's natural integer type (the type of its integer literals). The declaration of bench
        // was checked in load(), so the result is always an int.
        if( n > INT_MAX ) {
            throw std::out_of_range( "n does not fit into an AngelScript int" );
        }
        check( mContext->Prepare( mBench ), "Prepare()" );
        check( mContext->SetArgDWord( 0, static_cast<asDWORD>(static_cast<int>(n)) ), "SetArgDWord()" );
        int const r = mContext->Execute();
        if( r != asEXECUTION_FINISHED ) {
            if( r == asEXECUTION_EXCEPTION ) {
                throw std::runtime_error( "AngelScript exception in " + std::string( mContext->GetExceptionFunction()->GetDeclaration() )
                                        + " at line " + std::to_string( mContext->GetExceptionLineNumber() ) + ": "
                                        + mContext->GetExceptionString() );
            }
            throw std::runtime_error( "the execution of bench() did not finish, state " + std::to_string( r ) );
        }
        return static_cast<int>(mContext->GetReturnDWord());
    }

    void collect_garbage() override
    {
        mEngine->GarbageCollect( asGC_FULL_CYCLE );
    }

    void destroy_engine() override
    {
        mBench = nullptr;
        if( mContext != nullptr ) {
            mContext->Release();
            mContext = nullptr;
        }
        if( mEngine != nullptr ) {
            mEngine->ShutDownAndRelease();
            mEngine = nullptr;
        }
    }
};

} // namespace


int main( int argc, char *argv[] )
{
    csb::LanguageEntry entry;
    entry.id               = "angelscript";
    entry.default_variant  = default_variant;
    entry.headline_variant = headline_variant;
    for( auto const &variant : variants ) {
        entry.variants.emplace_back( variant.name );
    }
    entry.make = []( std::string const &name ) -> std::unique_ptr<csb::Language> {
        for( auto const &variant : variants ) {
            if( variant.name == name ) {
                return std::make_unique<AngelScriptLanguage>( variant );
            }
        }
        throw std::invalid_argument( "unknown AngelScript variant: " + name );
    };

    return csb::run_main( argc, argv, entry, csb::all_benchmarks() );
}
