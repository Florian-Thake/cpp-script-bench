/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <csb/language.hpp>

#include <chrono>
#include <cstdint>
#include <exception>
#include <string>


namespace csb::host::engine_reset {

/// One sample of engine_reset, the same for every runner. Not measured: \param lang creates a fresh engine,
/// \param prepare runs (e.g. saves the state to reset to), the script \param source is loaded and bench( n ) is called
/// once, so the engine has run the script. Measured: \param reset, then the load of the script again (the sub-phases
/// "reset" and "load"). Not measured: bench( n ) as the check, destroying the engine.
/// \return the sample, with the result of bench( n ) after the reset as its checksum.
template< typename Prepare, typename Reset >
HostSample run( Language &lang, BenchmarkInfo const &info, std::int64_t const n, std::string const &source, Prepare &&prepare,
                Reset &&reset )
{
    using Clock = std::chrono::steady_clock;
    auto const seconds = []( Clock::duration const d ) { return std::chrono::duration<double>( d ).count(); };

    lang.create_engine();
    try {
        prepare();
        (void)lang.load( info, source, n );
        (void)lang.call_bench( n );

        auto const t0 = Clock::now();
        reset();
        auto const t1 = Clock::now();
        (void)lang.load( info, source, n );
        auto const t2 = Clock::now();

        auto const checksum = lang.call_bench( n );
        lang.destroy_engine();
        return { seconds( t2 - t0 ), { { "reset", seconds( t1 - t0 ) }, { "load", seconds( t2 - t1 ) } }, checksum };
    } catch( std::exception const & ) {
        lang.destroy_engine();
        throw;
    }
}

} // namespace csb::host::engine_reset
