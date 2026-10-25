/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include "host_to_script/host.hpp"

#include <cstdint>


namespace csb::reference {

namespace {

// Normal C++ functions in place of the script functions: the compiler may inline them, as it would in any C++ program.
int f0()
{
    return 1;
}

int f1( int const a )
{
    return a & 7;
}

int f3( int const a, int const b, int const c )
{
    return (a + b + c) & 7;
}

} // namespace


std::int64_t host_to_script( std::int64_t const n )
{
    return csb::host::host_to_script::run( n, f0, f1, f3 );
}

} // namespace csb::reference
