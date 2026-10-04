/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */

// The native C++ reference: calls the reference implementation of each benchmark directly.
// No engine, no script. The compiler and flags are part of the system info in the report.

#include "registry.hpp"

#include <csb/runner.hpp>


namespace {

class CppReference final : public csb::Language
{
    std::int64_t (*mFunc)( std::int64_t ) = nullptr;

public:
    std::string name() const override { return "C++"; }
    std::string version() const override { return "(native reference)"; }
    std::string script_extension() const override { return {}; }

    void create_engine() override {}

    std::vector<csb::Phase> load( csb::BenchmarkInfo const &info, std::string const & ) override
    {
        mFunc = info.reference;
        return {};
    }

    std::int64_t call_bench( std::int64_t const n ) override
    {
        return mFunc( csb::opaque( n ) );
    }

    void destroy_engine() override
    {
        mFunc = nullptr;
    }
};

} // namespace


int main( int argc, char *argv[] )
{
    csb::LanguageEntry const entry{
        "cpp",
        { "" },
        "",
        []( std::string const & ) -> std::unique_ptr<csb::Language> { return std::make_unique<CppReference>(); }
    };
    return csb::run_main( argc, argv, entry, csb::all_benchmarks() );
}
