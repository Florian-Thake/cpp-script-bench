// SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
// SPDX-License-Identifier: MIT

int bench( int n )
{
    int sum = 0;
    for( int a = 0; a < n; ++a ) {
        for( int b = 0; b < n; ++b ) {
            for( int c = 0; c < n; ++c ) {
                for( int d = 0; d < n; ++d ) {
                    sum = sum + ((a + b + c + d) & 7);
                }
            }
        }
    }
    return sum;
}
