/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include <cstdint>


namespace csb::reference {

namespace {

struct Point
{
    std::int64_t x = 0;
    std::int64_t y = 0;
    std::int64_t z = 0;
};

} // namespace

std::int64_t records( std::int64_t const n )
{
    Point p;
    for( std::int64_t i = 0; i < n; ++i ) {
        p.x = p.x + (i & 7);
        p.y = p.y + (p.x & 3);
        p.z = p.z + (p.y & 1);
    }
    return p.x + p.y + p.z;
}

} // namespace csb::reference
