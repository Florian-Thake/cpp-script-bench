// SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
// SPDX-License-Identifier: MIT

int bench( int n )
{
    array<bool> flags;
    flags.reserve( n + 1 );
    for( int k = 0; k <= n; ++k ) {
        flags.insertLast( true );
    }
    for( int i = 2; i * i <= n; ++i ) {
        if( flags[i] ) {
            for( int j = i * i; j <= n; j = j + i ) {
                flags[j] = false;
            }
        }
    }
    int count = 0;
    for( int k = 2; k <= n; ++k ) {
        if( flags[k] ) {
            count = count + 1;
        }
    }
    return count;
}
