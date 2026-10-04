/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include "stats.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>


namespace csb {

Stats compute_stats( std::vector<double> samples )
{
    Stats s;
    if( samples.empty() ) {
        return s;
    }

    std::sort( samples.begin(), samples.end() );
    auto const count = samples.size();

    s.min    = samples.front();
    s.max    = samples.back();
    s.median = count % 2 == 1 ? samples[count / 2] : (samples[count / 2 - 1] + samples[count / 2]) / 2.0;
    s.mean   = std::accumulate( samples.begin(), samples.end(), 0.0 ) / static_cast<double>(count);

    if( count > 1 ) {
        double sum_sq = 0.0;
        for( double const v : samples ) {
            sum_sq += (v - s.mean) * (v - s.mean);
        }
        s.stddev = std::sqrt( sum_sq / static_cast<double>(count - 1) );
    }

    return s;
}

} // namespace csb
