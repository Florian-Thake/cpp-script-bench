# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

# f0, f1 and f3 are C++ functions in the module 'host', which the host registers.
# The idiomatic fast form, like the math functions in math_builtins: the functions are imported
# into the module namespace, so there is no attribute lookup in the module host on every call.
from host import f0, f1, f3


def bench(n):
    total = 0
    for i in range(n):
        total = total + f0() + f1(i) + f3(i, 1, 2)
    return total
