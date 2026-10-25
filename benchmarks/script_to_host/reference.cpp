/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include "script_to_host/host.hpp"

#include <cstdint>


namespace csb::reference {

// Calls the host functions directly: the compiler may inline them, as it would in any C++ program.
std::int64_t script_to_host( std::int64_t const n )
{
    namespace host = csb::host::script_to_host;

    std::int64_t sum = 0;
    for( int i = 0; i < n; ++i ) {
        sum = sum + host::f0() + host::f1( i ) + host::f3( i, 1, 2 );
    }
    return sum;
}

} // namespace csb::reference
