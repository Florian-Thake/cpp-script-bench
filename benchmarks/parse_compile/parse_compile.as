// SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
// SPDX-License-Identifier: MIT

// A template: the runner generates the marked function n times, as fn0 .. fn<n-1> (see README.md).

int g( int x )
{
    return 2 * x + 1;
}

// csb:repeat
int fn__I__( int a )
{
    int sum = 0;
    for( int i = 0; i < a; ++i ) {
        int r = i % 3;
        if( r == 0 ) {
            sum = sum + g( i );
        } else {
            sum = sum - i * r;
        }
    }
    return sum + __I__;
}

// csb:end
int bench( int n )
{
    return fn0( n ) + fn__LAST__( n );
}
