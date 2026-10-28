/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include <cstdint>


namespace csb::reference {

// C++ has no engine to reset. The reference only gives the expected checksum: bench( n ) of the script.
std::int64_t engine_reset( std::int64_t const n )
{
    std::int64_t sum = 3;  // offset, the top-level variable of the script
    for( std::int64_t i = 0; i < n; ++i ) {
        sum = sum + (i & 7);
    }
    return sum;
}

} // namespace csb::reference
