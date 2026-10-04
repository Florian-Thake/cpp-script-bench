/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include <cstdint>


namespace csb::reference {

std::int64_t loop( std::int64_t const n )
{
    std::int64_t sum = 0;
    for( std::int64_t i = 0; i < n; ++i ) {
        sum = sum + (i & 7);
    }
    return sum;
}

} // namespace csb::reference
