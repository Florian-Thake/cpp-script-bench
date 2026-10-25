/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include "buffer/host.hpp"

#include <cstdint>


namespace csb::reference {

std::int64_t buffer( std::int64_t const n )
{
    namespace host = csb::host::buffer;

    // Like the runners, the image keeps its memory between the calls.
    static host::Image image;
    image.reset( n );
    int const size = static_cast<int>(n);
    for( int y = 0; y < size; ++y ) {
        for( int x = 0; x < size; ++x ) {
            image.pixels[y * size + x] = host::pixel( x, y );
        }
    }
    return host::result( n, n * n, image.bytes() );
}

} // namespace csb::reference
