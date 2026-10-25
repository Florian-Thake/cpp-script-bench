/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <cstdint>


namespace csb::host::host_to_script {

/// The host's loop of host_to_script, the same for every runner and the C++ reference. \param f0, \param f1 and
/// \param f3 call the script functions f0(), f1( a ) and f3( a, b, c ) in the language's normal way and return their
/// results as integers.
template< typename F0, typename F1, typename F3 >
std::int64_t run( std::int64_t const n, F0 &&f0, F1 &&f1, F3 &&f3 )
{
    std::int64_t sum = 0;
    for( int i = 0; i < n; ++i ) {
        sum = sum + f0() + f1( i ) + f3( i, 1, 2 );
    }
    return sum;
}

} // namespace csb::host::host_to_script
