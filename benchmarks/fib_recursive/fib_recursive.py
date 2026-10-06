# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

def fib(x):
    if x < 2:
        return x
    else:
        return fib(x - 1) + fib(x - 2)


def bench(n):
    return fib(n)
