/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>


// The host side of buffer: the image which the script fills, and the checksum of the host over it.
namespace csb::host::buffer {

/// The value of the pixel at \param x, \param y (no alpha, so it stays below 2^24), the same formula as in the scripts.
inline std::uint32_t pixel( int const x, int const y )
{
    return static_cast<std::uint32_t>((x & 255) + (y & 255) * 256 + ((x + y) & 255) * 65536);
}

/// The image of the host: n x n pixels of 32 bit, row by row. TeaScript uses a TeaScript Buffer instead, which the
/// host creates (a Buffer cannot refer to memory of the host).
struct Image
{
    std::vector<std::uint32_t>  pixels;

    /// Makes the image n x n pixels, all 0. Called before every call of bench( n ); keeps the memory of the last call.
    void reset( std::int64_t const n ) { pixels.assign( static_cast<std::size_t>(n * n), 0 ); }

    std::byte const *bytes() const { return reinterpret_cast<std::byte const *>(pixels.data()); }
};

/// The checksum of the host over \param count pixels at \param pixels (4 bytes each, host byte order): the sum of all
/// pixels, each weighted with (index & 7) + 1, so a pixel at a wrong place changes it.
inline std::int64_t checksum( std::byte const *const pixels, std::int64_t const count )
{
    std::int64_t sum = 0;
    for( std::int64_t i = 0; i < count; ++i ) {
        std::uint32_t value = 0;
        std::memcpy( &value, pixels + i * 4, sizeof( value ) );
        sum = sum + static_cast<std::int64_t>(value) * ((i & 7) + 1);
    }
    return sum;
}

/// The result of one call of bench( \param n ): checks \param script_result, which must be the number of pixels, and
/// \return the checksum of the host over the image at \param pixels.
inline std::int64_t result( std::int64_t const n, std::int64_t const script_result, std::byte const *const pixels )
{
    if( script_result != n * n ) {
        throw std::runtime_error( "bench() must return the number of pixels n * n, but returned " + std::to_string( script_result ) );
    }
    return checksum( pixels, n * n );
}

} // namespace csb::host::buffer
