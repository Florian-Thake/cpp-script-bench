/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include <cstdint>


namespace csb::reference {

std::int64_t var_lifecycle( std::int64_t const n )
{
    std::int64_t sum = 0;
    for( std::int64_t i = 0; i < n; ++i ) {
        std::int64_t const a = i & 7;
        std::int64_t b = a + 1;
        b = b * a;
        sum = sum + b;
    }
    return sum;
}

} // namespace csb::reference
