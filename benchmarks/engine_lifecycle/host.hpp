/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <csb/language.hpp>

#include <chrono>
#include <cstdint>


namespace csb::host::engine_lifecycle {

/// One sample of engine_lifecycle and engine_minimal, the same for every runner: \param n cycles of creating an engine
/// with \param create and destroying it with \param destroy. Both are measured; their sums are the sub-phases "create"
/// and "destroy". Every API reports a failure (an exception, or a null pointer which the runner turns into one).
/// \return the sample, with the number of engines as its checksum.
template< typename Create, typename Destroy >
HostSample run( std::int64_t const n, Create &&create, Destroy &&destroy )
{
    using Clock = std::chrono::steady_clock;
    Clock::duration created{};
    Clock::duration destroyed{};
    for( std::int64_t i = 0; i < n; ++i ) {
        auto const t0 = Clock::now();
        create();
        auto const t1 = Clock::now();
        destroy();
        auto const t2 = Clock::now();
        created   += t1 - t0;
        destroyed += t2 - t1;
    }

    auto const seconds = []( Clock::duration const d ) { return std::chrono::duration<double>( d ).count(); };
    return { seconds( created + destroyed ), { { "create", seconds( created ) }, { "destroy", seconds( destroyed ) } }, n };
}

} // namespace csb::host::engine_lifecycle
