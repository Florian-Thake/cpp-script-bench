/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include <cmath>
#include <cstdint>


namespace csb::reference {

std::int64_t math_builtins( std::int64_t const n )
{
    double total = 0.0;
    for( std::int64_t i = 1; i <= n; ++i ) {
        total = total + std::floor( std::sqrt( static_cast<double>(i) ) );
    }
    return static_cast<std::int64_t>(total);
}

} // namespace csb::reference
