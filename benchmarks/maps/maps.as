// SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
// SPDX-License-Identifier: MIT

int bench( int n )
{
    array<string> keys;
    keys.reserve( n );
    for( int i = 0; i < n; ++i ) {
        keys.insertLast( "k" + i );
    }
    dictionary m;
    for( int i = 0; i < n; ++i ) {
        m[keys[i]] = i;
    }
    int sum = 0;
    for( int i = 0; i < n; ++i ) {
        sum = sum + int( m[keys[i]] );
    }
    for( int i = 0; i < n; i = i + 2 ) {
        m.delete( keys[i] );
    }
    for( int i = 0; i < n; ++i ) {
        if( m.exists( keys[i] ) ) {
            sum = sum + 1;
        }
    }
    return sum;
}
