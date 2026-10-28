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

/// \return whether \param info uses the binding layer of a language (a C++ binding library or a C API): the interop
/// benchmarks, and the host API benchmarks which register C++ in an engine or call script functions. The c-api
/// variants of Lua and Python run only these.
bool uses_binding( BenchmarkInfo const &info );

} // namespace csb
