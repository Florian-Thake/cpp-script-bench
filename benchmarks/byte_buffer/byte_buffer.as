// SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
// SPDX-License-Identifier: MIT

int bench( int n )
{
    array<uint8> buf( n );
    int x = 1;
    for( int i = 0; i < n; ++i ) {
        x = (x * 75 + 74) % 65537;
        buf[i] = uint8( x & 255 );
    }
    int a = 1;
    int b = 0;
    for( int i = 0; i < n; ++i ) {
        a = (a + buf[i]) % 65521;
        b = (b + a) % 65521;
    }
    return a + b;
}
