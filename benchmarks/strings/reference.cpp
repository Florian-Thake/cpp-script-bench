/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include <cstdint>
#include <string>


namespace csb::reference {

std::int64_t strings( std::int64_t const n )
{
    std::int64_t sum = 0;
    std::string prev;
    for( std::int64_t i = 0; i < n; ++i ) {
        std::string const s = "item" + std::to_string( i );
        if( s < prev ) {
            sum = sum + 1;
        }
        sum = sum + static_cast<std::int64_t>(s.size());
        std::string const d = s.substr( 4 );
        sum = sum + (std::stoll( d ) & 7);
        prev = s;
    }
    return sum;
}

} // namespace csb::reference
