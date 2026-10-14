/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include <cstdint>


namespace csb::reference {

namespace {

std::int64_t tak( std::int64_t const x, std::int64_t const y, std::int64_t const z )
{
    if( y < x ) {
        return tak( tak( x - 1, y, z ), tak( y - 1, z, x ), tak( z - 1, x, y ) );
    } else {
        return z;
    }
}

} // namespace


std::int64_t tak( std::int64_t const n )
{
    return tak( 3 * n, 2 * n, n );
}

} // namespace csb::reference
