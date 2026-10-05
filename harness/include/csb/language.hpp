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


/// The interface every language adapter implements.
/// One instance represents one variant of one language, e.g. TeaScript in the TeaStackVM with O1.
/// The runner calls the phases in this order:
///   create_engine(), load(), call_bench() repeatedly, destroy_engine().
/// Every phase may throw a std::exception on errors.
class Language
{
public:
    virtual ~Language() = default;

    virtual std::string name() const = 0;              ///< e.g. "TeaScript"
    virtual std::string version() const = 0;           ///< version of the language library, incl. git info if known
    virtual std::string script_extension() const = 0;  ///< e.g. ".tea"; empty if no script is used (C++ reference)

    /// Creates a fresh engine including its standard library. Measured as "startup".
    virtual void create_engine() = 0;

    /// Makes \param source runnable: parse/compile it and run its top level code, which defines bench( n ).
    /// \param source stays valid until destroy_engine() was called. Measured as "load" in total.
    /// \return optional sub-phase timings, measured by the adapter itself.
    virtual std::vector<Phase> load( BenchmarkInfo const &info, std::string const &source ) = 0;

    /// Calls bench( n ) once and returns its checksum. Measured as "execute".
    virtual std::int64_t call_bench( std::int64_t n ) = 0;

    /// Runs a full garbage collection if the language has one. Called before each call_bench(), not measured.
    virtual void collect_garbage() {}

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
