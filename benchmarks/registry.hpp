/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <csb/benchmark.hpp>

#include <span>


namespace csb {

/// All benchmark disciplines, in report order.
std::span<BenchmarkInfo const> all_benchmarks();

} // namespace csb
