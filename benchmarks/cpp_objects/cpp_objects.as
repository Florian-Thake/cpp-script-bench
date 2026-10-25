// SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
// SPDX-License-Identifier: MIT

// Vec3i is a C++ type, which the host binds as a value type: a default constructor and the
// fields int x, int y and int z.
int bench( int n )
{
    Vec3i p;
    for( int i = 0; i < n; ++i ) {
        p.x = p.x + (i & 7);
        p.y = p.y + (p.x & 3);
        p.z = p.z + (p.y & 1);
    }
    return p.x + p.y + p.z;
}
