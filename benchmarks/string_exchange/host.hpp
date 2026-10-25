/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <cstdint>
#include <string>


namespace csb::host::string_exchange {

/// The host's loop of string_exchange, the same for every runner and the C++ reference. \param f calls the script
/// function f( s ) with the C++ string s in the language's normal way and returns its result as a std::string.
/// The checksum adds the length and the last digit of every result.
template< typename F >
std::int64_t run( std::int64_t const n, F &&f )
{
    std::int64_t sum = 0;
    for( int i = 0; i < n; ++i ) {
        std::string const s = "item" + std::to_string( i );
        std::string const r = f( s );
        sum = sum + static_cast<std::int64_t>(r.size()) + (r.empty() ? 0 : r.back() - '0');
    }
    return sum;
}

} // namespace csb::host::string_exchange
