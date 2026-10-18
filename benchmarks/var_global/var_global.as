// SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
// SPDX-License-Identifier: MIT

// The script-level variable.
int sum = 0;

int bench( int n )
{
    sum = 0;
    for( int i = 0; i < n; ++i ) {
        sum = sum + (i & 7);
    }
    return sum;
}
