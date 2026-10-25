/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <cstdint>
#include <stdexcept>
#include <vector>


namespace csb::host::array_exchange {

/// The number of elements which every call of the host's loop passes in each direction: volume / n round trips with
/// arrays of n elements, so the series over n shows the cost per call against the cost per element.
constexpr std::int64_t volume = 40'000;

/// The host's loop of array_exchange, the same for every runner and the C++ reference. \param f converts the
/// std::vector<int> into the script's array, calls the script function f( a ) in the language's normal way and
/// converts its result back into a std::vector<int>. The checksum is the sum of all elements of all results.
template< typename F >
std::int64_t run( std::int64_t const n, F &&f )
{
    std::vector<int> a;
    a.reserve( static_cast<std::size_t>(n) );
    for( int i = 0; i < n; ++i ) {
        a.push_back( i & 7 );
    }
    std::int64_t sum = 0;
    for( std::int64_t round = 0; round < volume / n; ++round ) {
        std::vector<int> const b = f( a );
        if( b.size() != a.size() ) {
            throw std::runtime_error( "f( a ) must return an array of the same size" );
        }
        for( int const value : b ) {
            sum = sum + value;
        }
    }
    return sum;
}

} // namespace csb::host::array_exchange
