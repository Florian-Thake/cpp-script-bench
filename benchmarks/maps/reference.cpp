/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>


namespace csb::reference {

std::int64_t maps( std::int64_t const n )
{
    std::vector<std::string> keys;
    keys.reserve( static_cast<std::size_t>(n) );
    for( std::int64_t i = 0; i < n; ++i ) {
        keys.push_back( "k" + std::to_string( i ) );
    }
    std::unordered_map<std::string, std::int64_t> m;
    for( std::int64_t i = 0; i < n; ++i ) {
        m[keys[i]] = i;
    }
    std::int64_t sum = 0;
    for( std::int64_t i = 0; i < n; ++i ) {
        sum = sum + m[keys[i]];
    }
    for( std::int64_t i = 0; i < n; i = i + 2 ) {
        m.erase( keys[i] );
    }
    for( std::int64_t i = 0; i < n; ++i ) {
        if( m.contains( keys[i] ) ) {
            sum = sum + 1;
        }
    }
    return sum;
}

} // namespace csb::reference
