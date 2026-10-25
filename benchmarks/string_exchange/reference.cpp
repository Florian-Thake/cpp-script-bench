/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include "string_exchange/host.hpp"

#include <cstdint>
#include <string>


namespace csb::reference {

namespace {

// A normal C++ function in place of the script function: it returns its argument.
std::string f( std::string const &s )
{
    return s;
}

} // namespace


std::int64_t string_exchange( std::int64_t const n )
{
    return csb::host::string_exchange::run( n, f );
}

} // namespace csb::reference
