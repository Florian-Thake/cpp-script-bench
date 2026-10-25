/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include "registry.hpp"


// The C++ reference implementations, one per benchmark directory (<name>/reference.cpp).
namespace csb::reference {

std::int64_t fib_recursive( std::int64_t n );
std::int64_t loop( std::int64_t n );
std::int64_t int_arith( std::int64_t n );
std::int64_t mandelbrot( std::int64_t n );
std::int64_t math_builtins( std::int64_t n );
std::int64_t calls( std::int64_t n );
std::int64_t tak( std::int64_t n );
std::int64_t branching( std::int64_t n );
std::int64_t var_global( std::int64_t n );
std::int64_t var_nested( std::int64_t n );
std::int64_t var_lifecycle( std::int64_t n );
std::int64_t sieve( std::int64_t n );
std::int64_t sort( std::int64_t n );
std::int64_t byte_buffer( std::int64_t n );
std::int64_t maps( std::int64_t n );
std::int64_t strings( std::int64_t n );
std::int64_t records( std::int64_t n );
std::int64_t binary_trees( std::int64_t n );
std::int64_t script_to_host( std::int64_t n );
std::int64_t host_to_script( std::int64_t n );
std::int64_t cpp_objects( std::int64_t n );

} // namespace csb::reference


namespace csb {

namespace {

// The workload sizes. A series (more than one n) shows how the run time grows with n.
constexpr std::int64_t fib_recursive_n[] = { 15, 20, 25 };
constexpr std::int64_t loop_n[]          = { 1'000'000 };
constexpr std::int64_t int_arith_n[]     = { 5'000 };
constexpr std::int64_t mandelbrot_n[]    = { 100 };
constexpr std::int64_t math_builtins_n[] = { 100'000 };
constexpr std::int64_t calls_n[]         = { 100'000 };
constexpr std::int64_t tak_n[]           = { 4, 5, 6 };
constexpr std::int64_t branching_n[]     = { 300'000 };
constexpr std::int64_t var_global_n[]    = { 1'000'000 };
constexpr std::int64_t var_nested_n[]    = { 25 };
constexpr std::int64_t var_lifecycle_n[] = { 300'000 };
constexpr std::int64_t sieve_n[]         = { 40'000 };
constexpr std::int64_t sort_n[]          = { 1'000 };
constexpr std::int64_t byte_buffer_n[]   = { 100'000 };
constexpr std::int64_t maps_n[]          = { 50'000 };
constexpr std::int64_t strings_n[]       = { 70'000 };
constexpr std::int64_t records_n[]       = { 150'000 };
constexpr std::int64_t binary_trees_n[]  = { 4, 8, 12 };
constexpr std::int64_t script_to_host_n[] = { 100'000 };
constexpr std::int64_t host_to_script_n[] = { 100'000 };
constexpr std::int64_t cpp_objects_n[]    = { 150'000 };

} // namespace

std::span<BenchmarkInfo const> all_benchmarks()
{
    static constexpr BenchmarkInfo list[] = {
        { "fib_recursive", "core", "Recursive Fibonacci: function call and recursion overhead", fib_recursive_n, &reference::fib_recursive },
        { "loop",          "core", "Counting loop with a minimal body", loop_n, &reference::loop },
        { "int_arith",     "core", "Collatz step counts: integer modulo and division", int_arith_n, &reference::int_arith },
        { "mandelbrot",    "core", "Mandelbrot set: floating point arithmetic in nested loops", mandelbrot_n, &reference::mandelbrot },
        { "math_builtins", "core", "Calls of the built-in math functions sqrt and floor", math_builtins_n, &reference::math_builtins },
        { "calls",         "core", "Calls of tiny script functions with 0, 1 and 3 arguments", calls_n, &reference::calls },
        { "tak",           "core", "Tak function: recursion with 3 arguments and calls as arguments", tak_n, &reference::tak },
        { "branching",     "core", "If/else chain and short-circuit logic in a loop", branching_n, &reference::branching },
        { "var_global",    "data", "The loop of 'loop' with a script-level (global) accumulator", var_global_n, &reference::var_global },
        { "var_nested",    "data", "Four nested loops: reads of variables of four enclosing scopes", var_nested_n, &reference::var_nested },
        { "var_lifecycle", "data", "New block-local variables in every loop iteration", var_lifecycle_n, &reference::var_lifecycle },
        { "sieve",         "data", "Sieve of Eratosthenes on an array built by appending", sieve_n, &reference::sieve },
        { "sort",          "data", "Insertion sort by hand of generated values", sort_n, &reference::sort },
        { "byte_buffer",   "data", "Fill a byte buffer, then an Adler-32 style checksum over it", byte_buffer_n, &reference::byte_buffer },
        { "maps",          "data", "Maps with string keys: insert, lookup, remove, membership", maps_n, &reference::maps },
        { "strings",       "data", "Number -> string, comparison, length, substring, string -> number", strings_n, &reference::strings },
        { "records",       "data", "Field reads and writes of one record", records_n, &reference::records },
        { "binary_trees",  "data", "Build and check binary trees of records: allocation and garbage collection", binary_trees_n, &reference::binary_trees },
        { "script_to_host", "interop", "The loop of 'calls', but the functions are registered C++ functions", script_to_host_n, &reference::script_to_host },
        { "host_to_script", "interop", "The mirror of 'script_to_host': a C++ loop calls the script functions of 'calls'", host_to_script_n, &reference::host_to_script },
        { "cpp_objects",   "interop", "The loop of 'records' on a bound C++ type", cpp_objects_n, &reference::cpp_objects },
    };
    return list;
}

} // namespace csb
