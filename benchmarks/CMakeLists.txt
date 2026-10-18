# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

# The benchmark registry and the C++ reference implementations, linked into every runner.
# The scripts are not compiled; the runners load them at runtime from this directory.
add_library( csb_benchmarks STATIC
    registry.cpp
    fib_recursive/reference.cpp
    loop/reference.cpp
    int_arith/reference.cpp
    mandelbrot/reference.cpp
    math_builtins/reference.cpp
    calls/reference.cpp
    tak/reference.cpp
    branching/reference.cpp
    var_global/reference.cpp
    var_nested/reference.cpp
    var_lifecycle/reference.cpp
    sieve/reference.cpp
    sort/reference.cpp
    byte_buffer/reference.cpp
    maps/reference.cpp
    strings/reference.cpp )

target_include_directories( csb_benchmarks PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" )
target_link_libraries( csb_benchmarks PUBLIC csb_harness )

csb_apply_compiler_options( csb_benchmarks )
