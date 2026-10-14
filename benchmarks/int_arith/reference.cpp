/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include <cstdint>


namespace csb::reference {

std::int64_t int_arith( std::int64_t const n )
{
    std::int64_t total = 0;
    for( std::int64_t s = 1; s <= n; ++s ) {
        std::int64_t x = s;
        while( x != 1 ) {
            if( x % 2 == 0 ) {
                x = x / 2;
            } else {
                x = 3 * x + 1;
            }
            total = total + 1;
        }
    }
    return total;
}

} // namespace csb::reference
