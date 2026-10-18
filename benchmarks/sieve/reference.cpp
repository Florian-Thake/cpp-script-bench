/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include <cstddef>
#include <cstdint>
#include <vector>


namespace csb::reference {

std::int64_t sieve( std::int64_t const n )
{
    std::vector<bool> flags;
    flags.reserve( static_cast<std::size_t>(n + 1) );
    for( std::int64_t k = 0; k <= n; ++k ) {
        flags.push_back( true );
    }
    for( std::int64_t i = 2; i * i <= n; ++i ) {
        if( flags[i] ) {
            for( std::int64_t j = i * i; j <= n; j = j + i ) {
                flags[j] = false;
            }
        }
    }
    std::int64_t count = 0;
    for( std::int64_t k = 2; k <= n; ++k ) {
        if( flags[k] ) {
            count = count + 1;
        }
    }
    return count;
}

} // namespace csb::reference
