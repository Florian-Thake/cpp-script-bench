/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include "csb/language.hpp"
#include "system_info.hpp"

#include <cstdint>
#include <ostream>
#include <string>
#include <vector>


namespace csb {

/// The result of one benchmark for one language variant. All times in seconds.
struct BenchResult
{
    std::string          benchmark;
    std::string          group;
    std::int64_t         n        = 0;
    std::int64_t         expected = 0;
    std::int64_t         checksum = 0;
    std::string          status;          ///< "ok", "n/a", "wrong result" or "error: <message>"
    double               startup  = 0.0;  ///< create_engine()
    double               load     = 0.0;  ///< load() in total
    std::vector<Phase>   load_phases;     ///< sub-phases of load(), if available
    double               first    = 0.0;  ///< the very first call of bench( n )
    double               teardown = 0.0;  ///< destroy_engine()
    int                  warmup   = 0;    ///< the number of warm-up calls made
    std::vector<double>  samples;         ///< the measured calls of bench( n ) after warm-up
    bool                 budget_limited = false;  ///< the time budget ended the warm-up or the measured calls early

    bool ok() const { return status == "ok"; }
};

struct VariantResult
{
    std::string               variant;
    bool                      headline = false;  ///< the variant for the main comparison
    std::vector<BenchResult>  benchmarks;        ///< one entry per benchmark and n
};

struct Report
{
    std::string                 language_id;
    std::string                 language_name;
    std::string                 language_version;
    SystemInfo                  system;
    std::string                 timestamp;  ///< UTC, ISO 8601
    int                         warmup   = 0;
    int                         runs     = 0;
    int                         budget   = 0;  ///< seconds, 0 == unlimited
    int                         min_runs = 0;
    std::vector<VariantResult>  variants;
};

void write_json( Report const &report, std::ostream &out );

} // namespace csb
