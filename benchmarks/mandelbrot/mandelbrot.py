# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

def bench(n):
    max_iter = 50
    count = 0
    for y in range(n):
        ci = -1.25 + 2.5 * y / n
        for x in range(n):
            cr = -2.0 + 2.5 * x / n
            zr = 0.0
            zi = 0.0
            k = 0
            while k < max_iter and zr * zr + zi * zi <= 4.0:
                t = zr * zr - zi * zi + cr
                zi = 2.0 * zr * zi + ci
                zr = t
                k = k + 1
            count = count + k
    return count
