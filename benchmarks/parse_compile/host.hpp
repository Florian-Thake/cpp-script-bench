/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <csb/language.hpp>

#include <cstdint>
#include <exception>
#include <string>
#include <utility>


namespace csb::host::parse_compile {

/// One sample of parse_compile, the same for every runner: \param lang loads the generated \param source into a fresh
/// engine. Only load() is measured: the sum of its sub-phases (parse, compile, top level), i.e. "source -> runnable".
/// Creating the engine, the check with bench( n ) and destroying the engine are not measured.
/// \return the sample, with the result of bench( n ) as its checksum.
inline HostSample run( Language &lang, BenchmarkInfo const &info, std::int64_t const n, std::string const &source )
{
    lang.create_engine();
    try {
        auto phases = lang.load( info, source, n );
        auto const checksum = lang.call_bench( n );
        lang.destroy_engine();

        double seconds = 0.0;
        for( auto const &phase : phases ) {
            seconds += phase.seconds;
        }
        return { seconds, std::move( phases ), checksum };
    } catch( std::exception const & ) {
        lang.destroy_engine();
        throw;
    }
}

} // namespace csb::host::parse_compile
