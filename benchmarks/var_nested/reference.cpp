/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include <cstdint>


namespace csb::reference {

std::int64_t var_nested( std::int64_t const n )
{
    std::int64_t sum = 0;
    for( std::int64_t a = 0; a < n; ++a ) {
        for( std::int64_t b = 0; b < n; ++b ) {
            for( std::int64_t c = 0; c < n; ++c ) {
                for( std::int64_t d = 0; d < n; ++d ) {
                    sum = sum + ((a + b + c + d) & 7);
                }
            }
        }
    }
    return sum;
}

} // namespace csb::reference
