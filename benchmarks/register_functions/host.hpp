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
#include <string_view>
#include <vector>


namespace csb::host::register_functions {

/// The names of the registrations of register_functions and register_variables: \param prefix followed by 0 .. n-1.
/// The runners create them before the measurement.
inline std::vector<std::string> names( std::string_view const prefix, std::int64_t const n )
{
    std::vector<std::string> res;
    res.reserve( static_cast<size_t>(n) );
    for( std::int64_t i = 0; i < n; ++i ) {
        res.push_back( std::string( prefix ) + std::to_string( i ) );
    }
    return res;
}

/// The values of the host variables of register_variables: variable k has the value k.
inline std::vector<int> values( std::int64_t const n )
{
    std::vector<int> res;
    res.reserve( static_cast<size_t>(n) );
    for( std::int64_t i = 0; i < n; ++i ) {
        res.push_back( static_cast<int>(i) );
    }
    return res;
}

/// One sample of register_functions and register_variables, the same for every runner: \param lang creates a fresh
/// engine, then \param prepare runs (e.g. the import of the module the registrations go into), then
/// \param register_all makes the n registrations. Only \param register_all is measured. Afterwards the generated
/// script of \param info, whose bench( n ) uses the first and the last registration, is the check.
/// \return the sample, with the result of bench( n ) as its checksum.
template< typename Prepare, typename Register >
HostSample run( Language &lang, BenchmarkInfo const &info, std::int64_t const n, std::string const &source, Prepare &&prepare,
                Register &&register_all )
{
    using Clock = std::chrono::steady_clock;
    lang.create_engine();
    try {
        prepare();
        auto const start = Clock::now();
        register_all();
        double const seconds = std::chrono::duration<double>( Clock::now() - start ).count();

        (void)lang.load( info, source, n );
        auto const checksum = lang.call_bench( n );
        lang.destroy_engine();
        return { seconds, {}, checksum };
    } catch( std::exception const & ) {
        lang.destroy_engine();
        throw;
    }
}

} // namespace csb::host::register_functions
