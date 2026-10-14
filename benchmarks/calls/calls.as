// SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
// SPDX-License-Identifier: MIT

int f0()
{
    return 1;
}

int f1( int a )
{
    return a & 7;
}

int f3( int a, int b, int c )
{
    return (a + b + c) & 7;
}

int bench( int n )
{
    int sum = 0;
    for( int i = 0; i < n; ++i ) {
        sum = sum + f0() + f1( i ) + f3( i, 1, 2 );
    }
    return sum;
}
