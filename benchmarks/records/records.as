// SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
// SPDX-License-Identifier: MIT

// The record type.
class Point
{
    int x = 0;
    int y = 0;
    int z = 0;
}

int bench( int n )
{
    Point p;
    for( int i = 0; i < n; ++i ) {
        p.x = p.x + (i & 7);
        p.y = p.y + (p.x & 3);
        p.z = p.z + (p.y & 1);
    }
    return p.x + p.y + p.z;
}
