/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include "register_functions/host.hpp"  // names()

#include <cstddef>
#include <cstdint>


namespace csb::host::call_by_name {

/// The number of calls of script functions per call of call_bench( n ).
constexpr std::int64_t calls = 100'000;

/// The host's loop of call_by_name and call_by_handle, the same for every runner and the C++ reference: \ref calls
/// calls, which cycle through the \param m script functions fn0 .. fn<m-1> (names: register_functions::names( "fn" )).
/// \param call( k, a ) calls fn<k>( a ) in the way of the benchmark (looked up by its name, or through the handle
/// looked up in load()) and returns its result as an integer.
template< typename Call >
std::int64_t run( std::int64_t const m, Call &&call )
{
    std::int64_t sum = 0;
    for( std::int64_t i = 0; i < calls; ++i ) {
        sum = sum + call( static_cast<std::size_t>(i % m), static_cast<int>(i & 7) );
    }
    return sum;
}

} // namespace csb::host::call_by_name
