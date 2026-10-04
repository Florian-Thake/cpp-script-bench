/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include "benchmark.hpp"
#include "language.hpp"

#include <span>


namespace csb {

/// The common main() of every runner executable.
/// Parses the command line, runs the selected benchmarks for the selected variants,
/// verifies every result against the C++ reference and prints/writes the report.
/// \return the process exit code.
int run_main( int argc, char *argv[], LanguageEntry const &language, std::span<BenchmarkInfo const> benchmarks );

} // namespace csb
