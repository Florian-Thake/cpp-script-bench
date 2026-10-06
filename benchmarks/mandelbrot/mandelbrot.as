// SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
// SPDX-License-Identifier: MIT

int bench( int n )
{
    const int max_iter = 50;
    int count = 0;
    for( int y = 0; y < n; ++y ) {
        double ci = -1.25 + 2.5 * y / n;
        for( int x = 0; x < n; ++x ) {
            double cr = -2.0 + 2.5 * x / n;
            double zr = 0.0;
            double zi = 0.0;
            int k = 0;
            while( k < max_iter && zr * zr + zi * zi <= 4.0 ) {
                double t = zr * zr - zi * zi + cr;
                zi = 2.0 * zr * zi + ci;
                zr = t;
                k = k + 1;
            }
            count = count + k;
        }
    }
    return count;
}
