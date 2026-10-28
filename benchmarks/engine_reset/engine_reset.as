// SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
// SPDX-License-Identifier: MIT

// The script of engine_reset: a top-level variable and two functions. The runner loads it, resets
// the engine and loads it again (see README.md).

int offset = 3;

int f1( int a )
{
    return a & 7;
}

int bench( int n )
{
    int sum = offset;
    for( int i = 0; i < n; ++i ) {
        sum = sum + f1( i );
    }
    return sum;
}
