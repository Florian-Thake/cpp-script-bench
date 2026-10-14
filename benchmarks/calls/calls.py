# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

def f0():
    return 1


def f1(a):
    return a & 7


def f3(a, b, c):
    return (a + b + c) & 7


def bench(n):
    total = 0
    for i in range(n):
        total = total + f0() + f1(i) + f3(i, 1, 2)
    return total
