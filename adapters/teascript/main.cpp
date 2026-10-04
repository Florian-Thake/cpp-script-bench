/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */

// The TeaScript runner. Variants:
//   vm-O0, vm-O1, vm-O2 : compiled and executed in the TeaStackVM with the given optimization level.
//   ast                 : the parsed AST is evaluated directly (no compilation).

#include "registry.hpp"

#include <csb/runner.hpp>

#include <teascript/Engine.hpp>
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


class TeaScriptLanguage final : public csb::Language
{
    Variant                       mVariant;
    std::unique_ptr<BenchEngine>  mEngine;
    teascript::ASTNodePtr         mAst;  // kept alive as long as the engine, the defined functions may refer to it.

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

    std::vector<csb::Phase> load( csb::BenchmarkInfo const &info, std::string const &source ) override
    {
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
    }

    std::int64_t call_bench( std::int64_t const n ) override
    {
        return mEngine->CallFuncEx( "bench", static_cast<teascript::Integer>(n) ).GetAsInteger();
    }

    void destroy_engine() override
    {
        mEngine.reset();
        mAst.reset();
    }
};

} // namespace


int main( int argc, char *argv[] )
{
    csb::LanguageEntry entry;
    entry.id              = "teascript";
    entry.default_variant = CSB_TEASCRIPT_DEFAULT_VARIANT;
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
