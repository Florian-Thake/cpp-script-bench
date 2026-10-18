/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include <cstddef>
#include <cstdint>
#include <vector>


namespace csb::reference {

std::int64_t byte_buffer( std::int64_t const n )
{
    std::vector<std::uint8_t> buf( static_cast<std::size_t>(n) );
    std::int64_t x = 1;
    for( std::int64_t i = 0; i < n; ++i ) {
        x = (x * 75 + 74) % 65537;
        buf[i] = static_cast<std::uint8_t>(x & 255);
    }
    std::int64_t a = 1;
    std::int64_t b = 0;
    for( std::int64_t i = 0; i < n; ++i ) {
        a = (a + buf[i]) % 65521;
        b = (b + a) % 65521;
    }
    return a + b;
}

} // namespace csb::reference
