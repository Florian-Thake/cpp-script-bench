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

std::span<BenchmarkInfo const> all_benchmarks()
{
    static constexpr BenchmarkInfo list[] = {
        { "fib_recursive", "core", "Recursive Fibonacci: function call and recursion overhead", 25, &reference::fib_recursive },
        { "loop",          "core", "Counting loop with a minimal body", 1'000'000, &reference::loop },
        { "mandelbrot",    "core", "Mandelbrot set: floating point arithmetic in nested loops", 100, &reference::mandelbrot },
    };
    return list;
}

} // namespace csb
