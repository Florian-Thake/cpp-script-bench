/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <cstdint>
#include <span>
#include <string_view>


namespace csb {

/// Describes one benchmark discipline.
/// The scripts live in benchmarks/<name>/<name><ext>. The C++ reference implementation is compiled into
/// every runner. It is the "C++" entry of the comparison and also computes the expected checksum for
/// verifying the script results.
struct BenchmarkInfo
{
    std::string_view               name;         ///< directory and file base name below benchmarks/
    std::string_view               group;        ///< discipline group, e.g. "core", "data", "interop", "host"
    std::string_view               description;
    std::span<std::int64_t const>  series;       ///< workload sizes passed to bench( n ), each one is a separate run; usually only one
    std::int64_t                 (*reference)( std::int64_t n );  ///< C++ reference implementation, returns the expected checksum
    bool                           has_script = true;  ///< false: the benchmark needs no script (e.g. engine_lifecycle)
};


/// Returns \param v unchanged, but hides the value from the optimizer.
/// Used so the C++ reference cannot constant fold its workload.
template< typename T >
T opaque( T const v ) noexcept
{
    T volatile tmp = v;
    return tmp;
}

} // namespace csb
