// SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
// SPDX-License-Identifier: MIT

// The node type. A leaf has null handles.
class Node
{
    Node@ left;
    Node@ right;
    Node( Node@ l, Node@ r ) { @left = l; @right = r; }
}

Node@ make( int d )
{
    if( d == 0 ) {
        return Node( null, null );
    }
    return Node( make( d - 1 ), make( d - 1 ) );
}

int check( Node@ t )
{
    if( t.left is null ) {
        return 1;
    }
    return 1 + check( t.left ) + check( t.right );
}

int bench( int n )
{
    int sum = 0;
    int count = 1 << (16 - n);
    for( int k = 0; k < count; ++k ) {
        Node@ t = make( n );
        sum = sum + check( t );
    }
    return sum;
}
