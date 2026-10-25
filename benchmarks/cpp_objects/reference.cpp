/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include "cpp_objects/host.hpp"

#include <cstdint>


namespace csb::reference {

std::int64_t cpp_objects( std::int64_t const n )
{
    csb::host::cpp_objects::Vec3i p;
    for( int i = 0; i < n; ++i ) {
        p.x = p.x + (i & 7);
        p.y = p.y + (p.x & 3);
        p.z = p.z + (p.y & 1);
    }
    return p.x + p.y + p.z;
}

} // namespace csb::reference
