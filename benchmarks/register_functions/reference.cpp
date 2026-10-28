/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include "script_to_host/host.hpp"

#include <cstdint>


namespace csb::reference {

// C++ registers nothing. The reference only gives the expected checksum: bench( n ) of the script calls the first and
// the last registered function, both f1 of script_to_host.
std::int64_t register_functions( std::int64_t const n )
{
    int const a = static_cast<int>(n);
    return csb::host::script_to_host::f1( a ) + csb::host::script_to_host::f1( a + 3 );
}

} // namespace csb::reference
