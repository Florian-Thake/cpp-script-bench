# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

# A template: the runner generates the marked function n times, as fn0 .. fn<n-1> (see README.md).

def g(x):
    return 2 * x + 1


# csb:repeat
def fn__I__(a):
    total = 0
    for i in range(a):
        r = i % 3
        if r == 0:
            total = total + g(i)
        else:
            total = total - i * r
    return total + __I__


# csb:end
def bench(n):
    return fn0(n) + fn__LAST__(n)
