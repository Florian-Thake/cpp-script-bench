/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include <cstdint>


namespace csb::reference {

std::int64_t branching( std::int64_t const n )
{
    std::int64_t sum = 0;
    for( std::int64_t i = 0; i < n; ++i ) {
        std::int64_t const k = i & 7;
        if( k == 0 ) {
            sum = sum + 1;
        } else if( k == 1 ) {
            sum = sum + 2;
        } else if( k < 4 ) {
            sum = sum + 3;
        } else {
            sum = sum + 4;
        }
        if( (k > 2 && k < 6) || k == 7 ) {
            sum = sum + 5;
        }
    }
    return sum;
}

} // namespace csb::reference
