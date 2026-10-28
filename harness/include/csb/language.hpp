/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include "benchmark.hpp"

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>


namespace csb {

/// A named sub-phase of loading a script, e.g. "parse" or "compile".
/// Only reported for languages which can separate them.
struct Phase
{
    std::string  name;
    double       seconds = 0.0;
};


/// One sample of a host API benchmark (group "host"), measured by the adapter itself.
struct HostSample
{
    double              seconds  = 0.0;  ///< the measured operation in total
    std::vector<Phase>  phases;          ///< its sub-phases, if available
    std::int64_t        checksum = 0;    ///< shows that the operation worked, checked against the C++ reference
};


/// The interface every language adapter implements.
/// One instance represents one variant of one language, e.g. TeaScript in the TeaStackVM with O1.
/// The runner calls the phases in this order:
///   create_engine(), bind(), load(), call_bench() repeatedly, destroy_engine().
/// For the host API benchmarks (group "host") it only calls run_host() repeatedly instead.
/// Every phase may throw a std::exception on errors.
class Language
{
public:
    virtual ~Language() = default;

    virtual std::string name() const = 0;              ///< e.g. "TeaScript"
    virtual std::string version() const = 0;           ///< version of the language library, incl. git info if known
    virtual std::string script_extension() const = 0;  ///< e.g. ".tea"; empty if no script is used (C++ reference)

    /// \return false if this variant does not run \param info, which is then reported as "n/a". E.g. a variant
    /// which only differs in how C++ is bound runs only the interop benchmarks.
    virtual bool supports( BenchmarkInfo const &info ) const { (void)info; return true; }

    /// Creates a fresh engine including its standard library. Measured as "startup".
    virtual void create_engine() = 0;

    /// Registers what the script of \param info needs from the host: C++ functions, types and data (see
    /// benchmarks/<name>/host.hpp). Only the interop benchmarks need something. Measured as "bind".
    virtual void bind( BenchmarkInfo const &info ) { (void)info; }

    /// Makes \param source runnable: parse/compile it and run its top level code, which defines bench( n ).
    /// \param source stays valid until destroy_engine() was called. Measured as "load" in total.
    /// \return optional sub-phase timings, measured by the adapter itself.
    virtual std::vector<Phase> load( BenchmarkInfo const &info, std::string const &source ) = 0;

    /// Calls bench( n ) once and returns its checksum. Measured as "execute".
    virtual std::int64_t call_bench( std::int64_t n ) = 0;

    /// Runs a full garbage collection if the language has one. Called before each call_bench(), not measured.
    virtual void collect_garbage() {}

    /// Host API benchmarks: performs the C++ operation of \param info once with the size \param n and measures it.
    /// What a sample needs beforehand (e.g. a fresh engine) and the check of its result afterwards are not measured.
    /// \param source is the script of \param info, empty if it has none. Each call stands alone: no engine is left over.
    virtual HostSample run_host( BenchmarkInfo const &info, std::int64_t n, std::string const &source )
    {
        (void)n;
        (void)source;
        throw std::runtime_error( "run_host() is not implemented for " + std::string( info.name ) );
    }

    /// Destroys the engine. Measured as "teardown".
    virtual void destroy_engine() = 0;
};


/// Describes all variants of one language and how to create them. Each runner executable provides one.
struct LanguageEntry
{
    std::string               id;               ///< short id for result files, e.g. "teascript"
    std::vector<std::string>  variants;         ///< all variants; a single empty string if there is only one
    std::string               default_variant;  ///< used when no --variant is given
    std::function<std::unique_ptr<Language>( std::string const &variant )>  make;
    std::optional<std::string>  headline_variant;  ///< the variant for the main comparison (see PLAN.md), if this runner has it
};

} // namespace csb
