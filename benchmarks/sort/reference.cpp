/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include <cstddef>
#include <cstdint>
#include <vector>


namespace csb::reference {

std::int64_t sort( std::int64_t const n )
{
    std::vector<std::int64_t> a;
    a.reserve( static_cast<std::size_t>(n) );
    std::int64_t x = 1;
    for( std::int64_t k = 0; k < n; ++k ) {
        x = (x * 75 + 74) % 65537;
        a.push_back( x );
    }
    for( std::int64_t i = 1; i < n; ++i ) {
        std::int64_t const v = a[i];
        std::int64_t j = i - 1;
        while( j >= 0 && a[j] > v ) {
            a[j + 1] = a[j];
            j = j - 1;
        }
        a[j + 1] = v;
    }
    std::int64_t sum = 0;
    for( std::int64_t i = 0; i < n; ++i ) {
        sum = sum + a[i] * (i & 7);
    }
    return sum;
}

} // namespace csb::reference
