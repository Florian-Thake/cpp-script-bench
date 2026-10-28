/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include <cstdint>


namespace csb::reference {

// C++ registers nothing. The reference only gives the expected checksum: bench( n ) of the script adds the first and
// the last registered variable, whose values are 0 and n-1.
std::int64_t register_variables( std::int64_t const n )
{
    return 0 + (n - 1);
}

} // namespace csb::reference
