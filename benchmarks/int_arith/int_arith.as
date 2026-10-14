// SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
// SPDX-License-Identifier: MIT

int bench( int n )
{
    int total = 0;
    for( int s = 1; s <= n; ++s ) {
        int x = s;
        while( x != 1 ) {
            if( x % 2 == 0 ) {
                x = x / 2;
            } else {
                x = 3 * x + 1;
            }
            total = total + 1;
        }
    }
    return total;
}
