// SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
// SPDX-License-Identifier: MIT

int bench( int n )
{
    int sum = 0;
    string prev = "";
    for( int i = 0; i < n; ++i ) {
        string s = "item" + i;
        if( s < prev ) {
            sum = sum + 1;
        }
        sum = sum + int( s.length() );
        string d = s.substr( 4 );
        sum = sum + (int( parseInt( d ) ) & 7);
        prev = s;
    }
    return sum;
}
