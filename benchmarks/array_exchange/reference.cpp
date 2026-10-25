/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include "array_exchange/host.hpp"

#include <cstdint>
#include <vector>


namespace csb::reference {

namespace {

// A normal C++ function in place of the script function: a new array with every element + 1.
std::vector<int> f( std::vector<int> const &a )
{
    std::vector<int> b;
    b.reserve( a.size() );
    for( std::size_t i = 0; i < a.size(); ++i ) {
        b.push_back( a[i] + 1 );
    }
    return b;
}

} // namespace


std::int64_t array_exchange( std::int64_t const n )
{
    return csb::host::array_exchange::run( n, f );
}

} // namespace csb::reference
