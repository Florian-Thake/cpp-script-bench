// SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
// SPDX-License-Identifier: MIT

// f0, f1 and f3 are C++ functions, which the host registers:
// int f0(), int f1( int a ), int f3( int a, int b, int c )
int bench( int n )
{
    int sum = 0;
    for( int i = 0; i < n; ++i ) {
        sum = sum + f0() + f1( i ) + f3( i, 1, 2 );
    }
    return sum;
}
