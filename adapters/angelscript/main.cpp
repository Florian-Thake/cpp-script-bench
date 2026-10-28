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

#include "array_exchange/host.hpp"
#include "buffer/host.hpp"
#include "cpp_objects/host.hpp"
#include "engine_lifecycle/host.hpp"
#include "host_to_script/host.hpp"
#include "registry.hpp"
#include "script_to_host/host.hpp"
#include "string_exchange/host.hpp"

#include <csb/runner.hpp>

#include <angelscript.h>
#include <scriptarray/scriptarray.h>
#include <scriptdictionary/scriptdictionary.h>
#include <scriptmath/scriptmath.h>
#include <scriptstdstring/scriptstdstring.h>

#include <chrono>
#include <climits>
#include <cstring>
#include <initializer_list>
#include <memory>
#include <new>
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

/// The default constructor of the value type Vec3i of cpp_objects.
void construct_vec3i( void *const memory )
{
    new( memory ) csb::host::cpp_objects::Vec3i();
}

/// image[index] of buffer: a reference to the pixel, or a script exception if \param index is out of range (like the
/// index operator of the array add-on).
std::uint32_t &image_at( std::uint32_t const index, csb::host::buffer::Image &image )
{
    if( index >= image.pixels.size() ) {
        asGetActiveContext()->SetException( "Index out of bounds" );
        static std::uint32_t dummy = 0;
        return dummy;
    }
    return image.pixels[index];
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
    std::string_view     mBenchmark;          // the name of the loaded benchmark
    asIScriptFunction   *mF0      = nullptr;  // the script functions of host_to_script, looked up once like bench
    asIScriptFunction   *mF1      = nullptr;
    asIScriptFunction   *mF3      = nullptr;
    asIScriptFunction   *mStringF = nullptr;  // the script function f of string_exchange
    asIScriptFunction   *mArrayF  = nullptr;  // the script function f of array_exchange
    asITypeInfo         *mIntArrayType = nullptr;  // array<int>, looked up once
    csb::host::buffer::Image  mImage;         // buffer: the image of the host, the script's global property 'image'
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
        RegisterScriptMath( mEngine );  // the double versions of the functions (built with AS_USE_FLOAT=0)

        mContext = mEngine->CreateContext();
        if( mContext == nullptr ) {
            throw std::runtime_error( "CreateContext() failed" );
        }
    }

    void bind( csb::BenchmarkInfo const &info ) override
    {
        // The native calling convention, AngelScript's default on the supported platforms: the engine calls the C++
        // functions directly. (The generic convention asCALL_GENERIC is the fallback for the others.)
        if( info.name == "script_to_host" ) {
            namespace host = csb::host::script_to_host;
            check( mEngine->RegisterGlobalFunction( "int f0()", asFUNCTION( host::f0 ), asCALL_CDECL ), "RegisterGlobalFunction()" );
            check( mEngine->RegisterGlobalFunction( "int f1( int a )", asFUNCTION( host::f1 ), asCALL_CDECL ), "RegisterGlobalFunction()" );
            check( mEngine->RegisterGlobalFunction( "int f3( int a, int b, int c )", asFUNCTION( host::f3 ), asCALL_CDECL ),
                   "RegisterGlobalFunction()" );
        } else if( info.name == "cpp_objects" ) {
            // A value type, AngelScript's form for a small C++ struct: it lives on the script stack like an int, and its
            // fields are properties at their offsets.
            using csb::host::cpp_objects::Vec3i;
            check( mEngine->RegisterObjectType( "Vec3i", sizeof( Vec3i ), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<Vec3i>() ),
                   "RegisterObjectType()" );
            check( mEngine->RegisterObjectBehaviour( "Vec3i", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION( construct_vec3i ), asCALL_CDECL_OBJLAST ),
                   "RegisterObjectBehaviour()" );
            check( mEngine->RegisterObjectProperty( "Vec3i", "int x", asOFFSET( Vec3i, x ) ), "RegisterObjectProperty()" );
            check( mEngine->RegisterObjectProperty( "Vec3i", "int y", asOFFSET( Vec3i, y ) ), "RegisterObjectProperty()" );
            check( mEngine->RegisterObjectProperty( "Vec3i", "int z", asOFFSET( Vec3i, z ) ), "RegisterObjectProperty()" );
        } else if( info.name == "buffer" ) {
            // A reference type without reference counting (the host owns the object), with an index operator, registered
            // as a global property.
            check( mEngine->RegisterObjectType( "Image", 0, asOBJ_REF | asOBJ_NOCOUNT ), "RegisterObjectType()" );
            check( mEngine->RegisterObjectMethod( "Image", "uint &opIndex( uint )", asFUNCTION( image_at ), asCALL_CDECL_OBJLAST ),
                   "RegisterObjectMethod()" );
            check( mEngine->RegisterGlobalProperty( "Image image", &mImage ), "RegisterGlobalProperty()" );
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

        mBenchmark = info.name;
        if( mBenchmark == "host_to_script" ) {
            mF0 = find_function( mod, "int f0()" );
            mF1 = find_function( mod, "int f1( int )" );
            mF3 = find_function( mod, "int f3( int, int, int )" );
        } else if( mBenchmark == "string_exchange" ) {
            mStringF = find_function( mod, "string f( const string &in )" );
        } else if( mBenchmark == "array_exchange" ) {
            mArrayF       = find_function( mod, "array<int>@ f( const array<int>@ )" );
            mIntArrayType = mEngine->GetTypeInfoByDecl( "array<int>" );
        } else {
            mBench = find_function( mod, "int bench( int )" );
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
        if( mBenchmark == "host_to_script" ) {
            return csb::host::host_to_script::run(
                n,
                [&]() { return execute( mF0, {} ); },
                [&]( int const a ) { return execute( mF1, { a } ); },
                [&]( int const a, int const b, int const c ) { return execute( mF3, { a, b, c } ); } );
        }
        if( mBenchmark == "array_exchange" ) {
            return csb::host::array_exchange::run( n, [&]( std::vector<int> const &a ) { return execute( mArrayF, a ); } );
        }
        if( mBenchmark == "string_exchange" ) {
            return csb::host::string_exchange::run( n, [&]( std::string const &s ) { return execute( mStringF, s ); } );
        }
        if( mBenchmark == "buffer" ) {
            // The host resizes and clears its image, the script fills it, the host computes the checksum over it.
            mImage.reset( n );
            return csb::host::buffer::result( n, execute( mBench, { static_cast<int>(n) } ), mImage.bytes() );
        }
        return execute( mBench, { static_cast<int>(n) } );
    }

    void collect_garbage() override
    {
        mEngine->GarbageCollect( asGC_FULL_CYCLE );
    }

    void destroy_engine() override
    {
        mBench = nullptr;
        mF0    = nullptr;
        mF1    = nullptr;
        mF3    = nullptr;
        mStringF = nullptr;
        mArrayF  = nullptr;
        mIntArrayType = nullptr;
        if( mContext != nullptr ) {
            mContext->Release();
            mContext = nullptr;
        }
        if( mEngine != nullptr ) {
            mEngine->ShutDownAndRelease();
            mEngine = nullptr;
        }
    }

    csb::HostSample run_host( csb::BenchmarkInfo const &info, std::int64_t const n, std::string const & ) override
    {
        if( info.name == "engine_lifecycle" ) {
            return csb::host::engine_lifecycle::run( n, [&]() { create_engine(); }, [&]() { destroy_engine(); } );
        }
        if( info.name == "engine_minimal" ) {
            // The bare engine: no add-ons, no message callback, no context.
            return csb::host::engine_lifecycle::run(
                n,
                [&]() {
                    mEngine = asCreateScriptEngine();
                    if( mEngine == nullptr ) {
                        throw std::runtime_error( "asCreateScriptEngine() failed" );
                    }
                },
                [&]() {
                    mEngine->ShutDownAndRelease();
                    mEngine = nullptr;
                } );
        }
        throw std::runtime_error( "no host benchmark: " + std::string( info.name ) );
    }

private:
    /// \return the function with the declaration \param decl of \param mod.
    static asIScriptFunction *find_function( asIScriptModule *const mod, char const *const decl )
    {
        asIScriptFunction *const func = mod->GetFunctionByDecl( decl );
        if( func == nullptr ) {
            throw std::runtime_error( std::string( "the script does not define the function " ) + decl );
        }
        return func;
    }

    /// Calls the script function \param func with the int arguments \param args in the reused context.
    /// \return its int result.
    int execute( asIScriptFunction *const func, std::initializer_list<int> const args )
    {
        check( mContext->Prepare( func ), "Prepare()" );
        asUINT index = 0;
        for( int const arg : args ) {
            check( mContext->SetArgDWord( index++, static_cast<asDWORD>(arg) ), "SetArgDWord()" );
        }
        execute_prepared( func );
        return static_cast<int>(mContext->GetReturnDWord());
    }

    /// Calls the script function \param func, declared string f( const string &in ), with \param s in the reused
    /// context. \return a copy of its result.
    std::string execute( asIScriptFunction *const func, std::string const &s )
    {
        check( mContext->Prepare( func ), "Prepare()" );
        check( mContext->SetArgObject( 0, const_cast<std::string *>(&s) ), "SetArgObject()" );
        execute_prepared( func );
        return *static_cast<std::string const *>(mContext->GetReturnObject());
    }

    /// Calls the script function \param func, declared array<int>@ f( const array<int>@ ), with \param a in the reused
    /// context. \return its result. The elements of array<int> are stored contiguously, so they are copied as a block.
    std::vector<int> execute( asIScriptFunction *const func, std::vector<int> const &a )
    {
        auto const release = []( CScriptArray *const array ) { array->Release(); };
        std::unique_ptr<CScriptArray, decltype(release)> array( CScriptArray::Create( mIntArrayType, static_cast<asUINT>(a.size()) ), release );
        if( array == nullptr ) {
            throw std::runtime_error( "CScriptArray::Create() failed" );
        }
        std::memcpy( array->GetBuffer(), a.data(), a.size() * sizeof( int ) );
        check( mContext->Prepare( func ), "Prepare()" );
        check( mContext->SetArgObject( 0, array.get() ), "SetArgObject()" );
        execute_prepared( func );
        auto *const result = static_cast<CScriptArray *>(mContext->GetReturnObject());
        std::vector<int> b( result->GetSize() );
        std::memcpy( b.data(), result->GetBuffer(), b.size() * sizeof( int ) );
        return b;
    }

    /// Executes the prepared script function \param func and throws if it did not finish.
    void execute_prepared( asIScriptFunction *const func )
    {
        int const r = mContext->Execute();
        if( r != asEXECUTION_FINISHED ) {
            if( r == asEXECUTION_EXCEPTION ) {
                throw std::runtime_error( "AngelScript exception in " + std::string( mContext->GetExceptionFunction()->GetDeclaration() )
                                        + " at line " + std::to_string( mContext->GetExceptionLineNumber() ) + ": "
                                        + mContext->GetExceptionString() );
            }
            throw std::runtime_error( "the execution of " + std::string( func->GetName() ) + "() did not finish, state " + std::to_string( r ) );
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
