/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include <cstdint>
#include <memory>


namespace csb::reference {

namespace {

struct Node
{
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
};

std::unique_ptr<Node> make( std::int64_t const d )
{
    auto node = std::make_unique<Node>();
    if( d > 0 ) {
        node->left  = make( d - 1 );
        node->right = make( d - 1 );
    }
    return node;
}

std::int64_t check( Node const &t )
{
    if( t.left == nullptr ) {
        return 1;
    }
    return 1 + check( *t.left ) + check( *t.right );
}

} // namespace

std::int64_t binary_trees( std::int64_t const n )
{
    std::int64_t sum = 0;
    std::int64_t const count = std::int64_t( 1 ) << (16 - n);
    for( std::int64_t k = 0; k < count; ++k ) {
        auto const t = make( n );
        sum = sum + check( *t );
    }
    return sum;
}

} // namespace csb::reference
