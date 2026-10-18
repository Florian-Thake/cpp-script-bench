# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

def bench(n):
    buf = bytearray(n)
    x = 1
    for i in range(n):
        x = (x * 75 + 74) % 65537
        buf[i] = x & 255
    a = 1
    b = 0
    for i in range(n):
        a = (a + buf[i]) % 65521
        b = (b + a) % 65521
    return a + b
