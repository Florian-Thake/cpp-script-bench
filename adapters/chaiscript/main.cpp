/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */

// The ChaiScript runner, with the math module of ChaiScript_Extras. This file is built twice:
//   csb_chaiscript            ChaiScript in its default configuration, which is thread-safe (variant "default").
//   csb_chaiscript_nothreads  with CHAISCRIPT_NO_THREADS: no locking for multi-threaded use (variant "no-threads").

#include "csb_chaiscript_extras_git_version.h"
#include "csb_chaiscript_git_version.h"
#include "registry.hpp"

#include <csb/runner.hpp>

#include <chaiscript/chaiscript.hpp>
#include <chaiscript/extras/math.hpp>

#include <chrono>
#include <climits>
#include <functional>
#include <stdexcept>


namespace {

// The no-threads build is the headline, the closer match to the single-threaded engines of the others.
#if defined( CHAISCRIPT_NO_THREADS )
constexpr char const variant_name[] = "no-threads";
constexpr bool       is_headline    = true;
#else
constexpr char const variant_name[] = "default";
constexpr bool       is_headline    = false;
#endif


using Clock = std::chrono::steady_clock;

double seconds_since( Clock::time_point const start )
{
    return std::chrono::duration<double>( Clock::now() - start ).count();
}

/// Runs \param func and converts ChaiScript errors into std::runtime_error with a helpful message.
template< typename F >
auto translate_errors( F &&func ) -> decltype(func())
{
    try {
        return func();
    } catch( chaiscript::exception::eval_error const &ex ) {
        throw std::runtime_error( ex.pretty_print() );
    } catch( chaiscript::Boxed_Value const & ) {
        throw std::runtime_error( "a ChaiScript exception was thrown by the script" );
    }
}


class ChaiScriptLanguage final : public csb::Language
{
    std::unique_ptr<chaiscript::ChaiScript>        mChai;
    chaiscript::AST_NodePtr                        mAst;  // kept alive as long as the engine, the defined functions may refer to it.
    std::function<chaiscript::Boxed_Value( int )>  mBench;

public:
    std::string name() const override { return "ChaiScript"; }

    std::string version() const override
    {
        return chaiscript::Build_Info::version() + " (" CSB_CHAISCRIPT_GIT_VERSION "); ChaiScript_Extras (" CSB_CHAISCRIPT_EXTRAS_GIT_VERSION ")";
    }

    std::string script_extension() const override { return ".chai"; }

    void create_engine() override
    {
        // The full standard library, like the other languages get theirs.
        // Loading binary modules and external script files is disabled, so there is no disk IO.
        mChai = std::make_unique<chaiscript::ChaiScript>( std::vector<std::string>{}, std::vector<std::string>{},
                                                          std::vector<chaiscript::Options>{ chaiscript::Options::No_Load_Modules,
                                                                                            chaiscript::Options::No_External_Scripts } );
        // The standard library has no math functions. The math module of ChaiScript_Extras is ChaiScript's official
        // module for <cmath>, the counterpart of AngelScript's math add-on, so it belongs to the standard library here.
        mChai->add( chaiscript::extras::math::bootstrap() );
    }

    std::vector<csb::Phase> load( csb::BenchmarkInfo const &, std::string const &source ) override
    {
        return translate_errors( [&]() {
            std::vector<csb::Phase> phases;

            auto start = Clock::now();
            mAst = mChai->parse( source );
            phases.push_back( { "parse", seconds_since( start ) } );

            start = Clock::now();
            (void)mChai->eval( *mAst );
            phases.push_back( { "top_level", seconds_since( start ) } );

            mBench = mChai->eval<std::function<chaiscript::Boxed_Value( int )>>( "bench" );
            return phases;
        } );
    }

    std::int64_t call_bench( std::int64_t const n ) override
    {
        // int is the natural integer type of ChaiScript (the type of its integer literals).
        if( n > INT_MAX ) {
            throw std::out_of_range( "n does not fit into a ChaiScript int" );
        }
        return translate_errors( [&]() -> std::int64_t {
            auto const res = mBench( static_cast<int>(n) );
            // Only an int is accepted. Boxed_Number would also convert e.g. a double silently.
            if( not res.get_type_info().bare_equal( chaiscript::user_type<int>() ) ) {
                throw std::runtime_error( "bench() must return an int, but returned: " + mChai->get_type_name( res.get_type_info() ) );
            }
            return chaiscript::boxed_cast<int>( res );
        } );
    }

    void destroy_engine() override
    {
        mBench = nullptr;
        mChai.reset();
        mAst.reset();
    }
};

} // namespace


int main( int argc, char *argv[] )
{
    csb::LanguageEntry const entry{
        "chaiscript",
        { variant_name },
        variant_name,
        []( std::string const & ) -> std::unique_ptr<csb::Language> { return std::make_unique<ChaiScriptLanguage>(); },
        is_headline ? std::optional<std::string>( variant_name ) : std::nullopt
    };
    return csb::run_main( argc, argv, entry, csb::all_benchmarks() );
}
