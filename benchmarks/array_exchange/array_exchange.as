// SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
// SPDX-License-Identifier: MIT

// The host calls f( a ) in its loop with an array of integers and converts the result back (there
// is no bench). f returns a new array with every element + 1, built by appending.
array<int>@ f( const array<int>@ a )
{
    uint size = a.length();
    array<int> b;
    b.reserve( size );
    for( uint i = 0; i < size; ++i ) {
        b.insertLast( a[i] + 1 );
    }
    return b;
}
