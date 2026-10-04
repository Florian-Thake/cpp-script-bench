/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include <cstdint>


namespace csb::reference {

namespace {

std::int64_t fib( std::int64_t const x )
{
    if( x < 2 ) {
        return x;
    } else {
        return fib( x - 1 ) + fib( x - 2 );
    }
}

} // namespace


std::int64_t fib_recursive( std::int64_t const n )
{
    return fib( n );
}

} // namespace csb::reference
