/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include <cstdint>


namespace csb::reference {

namespace {

// Normal C++ functions: the compiler may inline them, as it would in any C++ program.
std::int64_t f0()
{
    return 1;
}

std::int64_t f1( std::int64_t const a )
{
    return a & 7;
}

std::int64_t f3( std::int64_t const a, std::int64_t const b, std::int64_t const c )
{
    return (a + b + c) & 7;
}

} // namespace


std::int64_t calls( std::int64_t const n )
{
    std::int64_t sum = 0;
    for( std::int64_t i = 0; i < n; ++i ) {
        sum = sum + f0() + f1( i ) + f3( i, 1, 2 );
    }
    return sum;
}

} // namespace csb::reference
