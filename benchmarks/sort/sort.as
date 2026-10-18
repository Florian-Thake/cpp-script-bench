// SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
// SPDX-License-Identifier: MIT

int bench( int n )
{
    array<int> a;
    a.reserve( n );
    int x = 1;
    for( int k = 0; k < n; ++k ) {
        x = (x * 75 + 74) % 65537;
        a.insertLast( x );
    }
    for( int i = 1; i < n; ++i ) {
        int v = a[i];
        int j = i - 1;
        while( j >= 0 && a[j] > v ) {
            a[j + 1] = a[j];
            j = j - 1;
        }
        a[j + 1] = v;
    }
    int sum = 0;
    for( int i = 0; i < n; ++i ) {
        sum = sum + a[i] * (i & 7);
    }
    return sum;
}
