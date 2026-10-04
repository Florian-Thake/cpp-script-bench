/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <vector>


namespace csb {

/// Summary of measured samples, all in seconds.
struct Stats
{
    double  min    = 0.0;
    double  median = 0.0;
    double  mean   = 0.0;
    double  stddev = 0.0;  ///< sample standard deviation, 0 for less than 2 samples
    double  max    = 0.0;
};

Stats compute_stats( std::vector<double> samples );

} // namespace csb
