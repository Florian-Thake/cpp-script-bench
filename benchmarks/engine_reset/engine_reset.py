# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

# The script of engine_reset: a top-level variable and two functions. The runner loads it, resets
# the engine and loads it again (see README.md).

offset = 3


def f1(a):
    return a & 7


def bench(n):
    total = offset
    for i in range(n):
        total = total + f1(i)
    return total
