/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include "call_by_name/host.hpp"

#include <cstddef>
#include <cstdint>


namespace csb::reference {

// C++ has no script functions to look up; the reference only gives the expected checksum. fn<k>( a ) = a + k.
std::int64_t call_by_name( std::int64_t const n )
{
    return csb::host::call_by_name::run( n, []( std::size_t const k, int const a ) { return a + static_cast<int>(k); } );
}

} // namespace csb::reference
