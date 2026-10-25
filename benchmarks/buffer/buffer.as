// SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
// SPDX-License-Identifier: MIT

// image is an object of the host with n * n pixels, all 0, of the registered type Image with the
// operator uint &opIndex( uint ).
int bench( int n )
{
    for( int y = 0; y < n; ++y ) {
        for( int x = 0; x < n; ++x ) {
            image[y * n + x] = (x & 255) + (y & 255) * 256 + ((x + y) & 255) * 65536;
        }
    }
    return n * n;
}
