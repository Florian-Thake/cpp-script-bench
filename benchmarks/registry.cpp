/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include "registry.hpp"


// The C++ reference implementations, one per benchmark directory (<name>/reference.cpp).
namespace csb::reference {

std::int64_t fib_recursive( std::int64_t n );
std::int64_t loop( std::int64_t n );
std::int64_t mandelbrot( std::int64_t n );

} // namespace csb::reference


namespace csb {

namespace {

// The workload sizes. A series (more than one n) shows how the run time grows with n.
constexpr std::int64_t fib_recursive_n[] = { 15, 20, 25 };
constexpr std::int64_t loop_n[]          = { 1'000'000 };
constexpr std::int64_t mandelbrot_n[]    = { 100 };

} // namespace

std::span<BenchmarkInfo const> all_benchmarks()
{
    static constexpr BenchmarkInfo list[] = {
        { "fib_recursive", "core", "Recursive Fibonacci: function call and recursion overhead", fib_recursive_n, &reference::fib_recursive },
        { "loop",          "core", "Counting loop with a minimal body", loop_n, &reference::loop },
        { "mandelbrot",    "core", "Mandelbrot set: floating point arithmetic in nested loops", mandelbrot_n, &reference::mandelbrot },
    };
    return list;
}

} // namespace csb
