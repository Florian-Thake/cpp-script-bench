# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

# image is an object of the host with n * n pixels of 32 bit, all 0. It supports the buffer
# protocol, so a memoryview of it (format 'I') writes directly into the memory of the host.
from host import image


def bench(n):
    buf = memoryview(image)
    for y in range(n):
        for x in range(n):
            buf[y * n + x] = (x & 255) + (y & 255) * 256 + ((x + y) & 255) * 65536
    return n * n
