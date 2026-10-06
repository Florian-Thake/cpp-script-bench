// SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
// SPDX-License-Identifier: MIT

int fib( int x )
{
    if( x < 2 ) {
        return x;
    } else {
        return fib( x - 1 ) + fib( x - 2 );
    }
}

int bench( int n )
{
    return fib( n );
}
