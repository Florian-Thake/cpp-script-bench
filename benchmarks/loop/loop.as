// SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
// SPDX-License-Identifier: MIT

int bench( int n )
{
    int sum = 0;
    for( int i = 0; i < n; ++i ) {
        sum = sum + (i & 7);
    }
    return sum;
}
