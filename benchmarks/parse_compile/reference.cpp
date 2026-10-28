/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include <cstdint>


namespace csb::reference {

namespace {

// The functions of the generated script. C++ has nothing to parse; the reference only gives the expected checksum.
int g( int const x )
{
    return 2 * x + 1;
}

/// The generated function f<k>.
int f( int const a, int const k )
{
    int sum = 0;
    for( int i = 0; i < a; ++i ) {
        int const r = i % 3;
        if( r == 0 ) {
            sum = sum + g( i );
        } else {
            sum = sum - i * r;
        }
    }
    return sum + k;
}

} // namespace


/// bench( n ) of the script with n generated functions: calls the first and the last one.
std::int64_t parse_compile( std::int64_t const n )
{
    int const a = static_cast<int>(n);
    return f( a, 0 ) + f( a, a - 1 );
}

} // namespace csb::reference
