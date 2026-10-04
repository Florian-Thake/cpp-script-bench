/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include <cstdint>


namespace csb::reference {

std::int64_t mandelbrot( std::int64_t const n )
{
    std::int64_t const max_iter = 50;
    std::int64_t       count    = 0;
    for( std::int64_t y = 0; y < n; ++y ) {
        double const ci = -1.25 + 2.5 * static_cast<double>(y) / static_cast<double>(n);
        for( std::int64_t x = 0; x < n; ++x ) {
            double const cr = -2.0 + 2.5 * static_cast<double>(x) / static_cast<double>(n);
            double       zr = 0.0;
            double       zi = 0.0;
            std::int64_t k  = 0;
            while( k < max_iter && zr * zr + zi * zi <= 4.0 ) {
                double const t = zr * zr - zi * zi + cr;
                zi = 2.0 * zr * zi + ci;
                zr = t;
                k  = k + 1;
            }
            count = count + k;
        }
    }
    return count;
}

} // namespace csb::reference
