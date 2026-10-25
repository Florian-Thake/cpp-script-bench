/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */

// The TeaScript runner. Variants:
//   vm-O0, vm-O1, vm-O2 : compiled and executed in the TeaStackVM with the given optimization level.
//   ast                 : the parsed AST is evaluated directly (no compilation).

#include "array_exchange/host.hpp"
#include "buffer/host.hpp"
#include "csb_teascript_git_version.h"
#include "host_to_script/host.hpp"
#include "registry.hpp"
#include "script_to_host/host.hpp"
#include "string_exchange/host.hpp"

#include <csb/runner.hpp>

#include <teascript/ControlFlow.hpp>
#include <teascript/Engine.hpp>
#include <teascript/Exception.hpp>
#include <teascript/Parser.hpp>
#include <teascript/StackVMCompiler.hpp>
#include <teascript/version.h>

#if TEASCRIPT_VERSION < TEASCRIPT_BUILD_VERSION_NUMBER( 0, 18, 0 )
# error cpp-script-bench needs TeaScript 0.18.0 or newer.
#endif

#include <chrono>
#include <stdexcept>


namespace {

struct Variant
{
    std::string_view      name;
    bool                  compile;    ///< true: compile and run in the TeaStackVM; false: evaluate the AST
    teascript::eOptimize  opt_level;
};

constexpr Variant variants[] = {
    { "vm-O0", true,  teascript::eOptimize::O0 },
    { "vm-O1", true,  teascript::eOptimize::O1 },
    { "vm-O2", true,  teascript::eOptimize::O2 },
    { "ast",   false, teascript::eOptimize::O0 },
};


/// Gives access to the Context for evaluating an AST directly.
class BenchEngine : public teascript::Engine
{
public:
    BenchEngine( teascript::Settings &&rSettings, eMode const mode )
        : teascript::Engine( std::move( rSettings ), mode )
    {
    }

    teascript::Context &GetContext() noexcept { return mContext; }
};


using Clock = std::chrono::steady_clock;

double seconds_since( Clock::time_point const start )
{
    return std::chrono::duration<double>( Clock::now() - start ).count();
}

/// Builds a one-line message with the category and, if known, the source location of \param ex
/// (like teascript::util::pretty_print(), which prints to stdout instead).
std::string error_message( teascript::exception::runtime_error const &ex )
{
    std::string msg = std::string( ex.GetCategory() ) + " error";
    if( ex.IsSourceLocSet() ) {
        msg += " at line " + std::to_string( ex.GetLine() ) + ", column " + std::to_string( ex.GetColumn() );
    }
    return msg + ": " + ex.ErrorStr_or_What();
}

/// Throws an eval_error if a user callback was called with another number of parameters than \param expected.
void check_param_count( teascript::Context const &rContext, size_t const expected, char const *const name )
{
    if( rContext.CurrentParamCount() != expected ) {
        throw teascript::exception::eval_error( rContext.GetCurrentSourceLocation(),
                                                std::string( name ) + ": expected " + std::to_string( expected ) + " parameters" );
    }
}

/// Registers the C++ functions of script_to_host as user callbacks, TeaScript's way to call C++ from a script.
void bind_script_to_host( teascript::Engine &rEngine )
{
    namespace host = csb::host::script_to_host;

    rEngine.RegisterUserCallback( "f0", []( teascript::Context &rContext ) -> teascript::ValueObject {
        check_param_count( rContext, 0, "f0" );
        return teascript::ValueObject( static_cast<teascript::Integer>(host::f0()) );
    } );
    rEngine.RegisterUserCallback( "f1", []( teascript::Context &rContext ) -> teascript::ValueObject {
        check_param_count( rContext, 1, "f1" );
        auto const a = rContext.ConsumeParam();
        return teascript::ValueObject( static_cast<teascript::Integer>(host::f1( static_cast<int>(a.GetAsInteger()) )) );
    } );
    rEngine.RegisterUserCallback( "f3", []( teascript::Context &rContext ) -> teascript::ValueObject {
        check_param_count( rContext, 3, "f3" );
        auto const a = rContext.ConsumeParam();
        auto const b = rContext.ConsumeParam();
        auto const c = rContext.ConsumeParam();
        return teascript::ValueObject( static_cast<teascript::Integer>(
            host::f3( static_cast<int>(a.GetAsInteger()), static_cast<int>(b.GetAsInteger()), static_cast<int>(c.GetAsInteger()) )) );
    } );
}

/// Runs \param func and converts TeaScript errors into std::runtime_error with a helpful message.
template< typename F >
auto translate_errors( F &&func ) -> decltype(func())
{
    try {
        return func();
    } catch( teascript::exception::runtime_error const &ex ) {
        throw std::runtime_error( error_message( ex ) );
    } catch( teascript::control::ControlBase const &ex ) {
        // Only possible when the AST is evaluated directly. Engine::EvaluateContent() converts them the same way.
        throw std::runtime_error( std::string( "a TeaScript control flow exception escaped: " ) + ex.what() );
    }
}


class TeaScriptLanguage final : public csb::Language
{
    Variant                       mVariant;
    std::unique_ptr<BenchEngine>  mEngine;
    teascript::ASTNodePtr         mAst;  // kept alive as long as the engine, the defined functions may refer to it.
    std::string_view              mBenchmark;  // the name of the loaded benchmark
    teascript::ValueObject        mImage;      // buffer: the Buffer of the host, shared with the script variable 'image'

public:
    explicit TeaScriptLanguage( Variant const &rVariant )
        : mVariant( rVariant )
    {
    }

    std::string name() const override { return "TeaScript"; }

    std::string version() const override
    {
        return std::to_string( TEASCRIPT_VERSION_MAJOR ) + "." + std::to_string( TEASCRIPT_VERSION_MINOR ) + "."
             + std::to_string( TEASCRIPT_VERSION_PATCH ) + " (" CSB_TEASCRIPT_GIT_VERSION ")";
    }

    std::string script_extension() const override { return ".tea"; }

    void create_engine() override
    {
        // The full Core Library, like the other languages get their full standard library.
        // The tsb path stays empty, so Bootstrap() neither reads nor writes compiled .tsb files (no disk IO).
        teascript::Settings settings( teascript::config::full(), mVariant.opt_level );
        mEngine = std::make_unique<BenchEngine>( std::move( settings ), mVariant.compile ? teascript::Engine::eMode::Compile
                                                                                         : teascript::Engine::eMode::Eval );
    }

    void bind( csb::BenchmarkInfo const &info ) override
    {
        translate_errors( [&]() {
            if( info.name == "script_to_host" ) {
                bind_script_to_host( *mEngine );
            } else if( info.name == "buffer" ) {
                // The image is a Buffer created by the host; the host keeps the shared value and resizes it before every
                // call. (A Buffer cannot refer to memory of the host.)
                mEngine->AddVar( "image", teascript::Buffer() );
                mImage = mEngine->GetVar( "image" );
            }
        } );
    }

    std::vector<csb::Phase> load( csb::BenchmarkInfo const &info, std::string const &source ) override
    {
        mBenchmark = info.name;
        return translate_errors( [&]() {
            std::vector<csb::Phase> phases;

            auto start = Clock::now();
            teascript::Parser  parser;
            mAst = parser.Parse( teascript::Content( source.data(), source.size() ), std::string( info.name ) );
            phases.push_back( { "parse", seconds_since( start ) } );

            if( mVariant.compile ) {
                start = Clock::now();
                teascript::StackVM::Compiler  compiler;
                auto const program = compiler.Compile( mAst, mVariant.opt_level );
                phases.push_back( { "compile", seconds_since( start ) } );

                start = Clock::now();
                (void)mEngine->ExecuteProgram( program );
                phases.push_back( { "top_level", seconds_since( start ) } );
            } else {
                start = Clock::now();
                (void)mAst->Eval( mEngine->GetContext() );
                phases.push_back( { "top_level", seconds_since( start ) } );
            }

            return phases;
        } );
    }

    std::int64_t call_bench( std::int64_t const n ) override
    {
        return translate_errors( [&]() -> std::int64_t {
            if( mBenchmark == "host_to_script" ) {
                return call_host_to_script( n );
            }
            if( mBenchmark == "array_exchange" ) {
                return csb::host::array_exchange::run( n, [&]( std::vector<int> const &a ) { return call_array_function( a ); } );
            }
            if( mBenchmark == "string_exchange" ) {
                // The string goes in as a String, the result comes back with GetAsString().
                return csb::host::string_exchange::run(
                    n, [&]( std::string const &s ) { return mEngine->CallFuncEx( "f", s ).GetAsString(); } );
            }
            if( mBenchmark == "buffer" ) {
                // The host resizes and clears its Buffer, the script fills it, the host computes the checksum over it.
                auto &image = mImage.GetValue<teascript::Buffer>();
                image.assign( static_cast<size_t>(n * n * 4), 0 );
                return csb::host::buffer::result( n, call_script_bench( n ), reinterpret_cast<std::byte const *>(image.data()) );
            }
            return call_script_bench( n );
        } );
    }

    void destroy_engine() override
    {
        mImage = teascript::ValueObject();
        mEngine.reset();
        mAst.reset();
    }

private:
    /// Calls bench( n ) of the script. \return its result.
    std::int64_t call_script_bench( std::int64_t const n )
    {
        auto const res = mEngine->CallFuncEx( "bench", static_cast<teascript::Integer>(n) );
        // Only an Integer is accepted. GetAsInteger() would also convert e.g. a f64 or a String silently.
        auto const *const p = res.GetValuePtr<teascript::Integer>();
        if( p == nullptr ) {
            throw std::runtime_error( "bench() must return an Integer, but returned: " + res.GetTypeInfo()->GetName() );
        }
        return static_cast<std::int64_t>(*p);
    }

    /// array_exchange: converts \param a into a Tuple, calls f with it and converts the result back.
    std::vector<int> call_array_function( std::vector<int> const &a )
    {
        // The elements are shared mutable values, like the ones _tuple_append creates.
        teascript::Tuple tuple;
        tuple.Reserve( a.size() );
        for( int const value : a ) {
            tuple.AppendValue( teascript::ValueObject( static_cast<teascript::Integer>(value),
                                                       teascript::ValueConfig( teascript::ValueShared, teascript::ValueMutable ) ) );
        }
        auto const res = mEngine->CallFuncEx( "f", std::move( tuple ) );
        auto const &result = res.GetValue<teascript::Tuple>();
        std::vector<int> b;
        b.reserve( result.Size() );
        for( std::size_t i = 0; i < result.Size(); ++i ) {
            b.push_back( static_cast<int>(result.GetValueByIdx( i ).GetAsInteger()) );
        }
        return b;
    }

    /// The host's loop of host_to_script: calls the script functions by name with CallFuncEx, TeaScript's way to call
    /// a script function from C++.
    std::int64_t call_host_to_script( std::int64_t const n )
    {
        return csb::host::host_to_script::run(
            n,
            [&]() { return mEngine->CallFuncEx( "f0" ).GetAsInteger(); },
            [&]( int const a ) { return mEngine->CallFuncEx( "f1", static_cast<teascript::Integer>(a) ).GetAsInteger(); },
            [&]( int const a, int const b, int const c ) {
                return mEngine->CallFuncEx( "f3", static_cast<teascript::Integer>(a), static_cast<teascript::Integer>(b),
                                            static_cast<teascript::Integer>(c) ).GetAsInteger();
            } );
    }
};

} // namespace


int main( int argc, char *argv[] )
{
    csb::LanguageEntry entry;
    entry.id               = "teascript";
    entry.default_variant  = CSB_TEASCRIPT_DEFAULT_VARIANT;
    entry.headline_variant = "vm-O1";  // independent of the default variant, which can be changed with CMake
    for( auto const &variant : variants ) {
        entry.variants.emplace_back( variant.name );
    }
    entry.make = []( std::string const &name ) -> std::unique_ptr<csb::Language> {
        for( auto const &variant : variants ) {
            if( variant.name == name ) {
                return std::make_unique<TeaScriptLanguage>( variant );
            }
        }
        throw std::invalid_argument( "unknown TeaScript variant: " + name );
    };

    return csb::run_main( argc, argv, entry, csb::all_benchmarks() );
}
