# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

def tak(x, y, z):
    if y < x:
        return tak(tak(x - 1, y, z), tak(y - 1, z, x), tak(z - 1, x, y))
    else:
        return z


def bench(n):
    return tak(3 * n, 2 * n, n)
