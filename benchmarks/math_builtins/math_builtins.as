// SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
// SPDX-License-Identifier: MIT

// sqrt and floor come from the math add-on, registered for double.
int bench( int n )
{
    double total = 0.0;
    for( int i = 1; i <= n; ++i ) {
        total = total + floor( sqrt( i ) );
    }
    return int( total );
}
