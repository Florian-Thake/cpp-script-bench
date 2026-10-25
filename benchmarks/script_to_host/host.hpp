/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#pragma once


// The C++ functions which every runner registers for the scripts of script_to_host. The C++ reference calls them
// directly. int is the natural integer type of the C++ side.
namespace csb::host::script_to_host {

inline int f0()
{
    return 1;
}

inline int f1( int const a )
{
    return a & 7;
}

inline int f3( int const a, int const b, int const c )
{
    return (a + b + c) & 7;
}

} // namespace csb::host::script_to_host
