# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

# The idiomatic fast form: the functions are imported into the module namespace, so there is no
# attribute lookup in the module math on every call.
from math import sqrt, floor


def bench(n):
    total = 0
    for i in range(1, n + 1):
        total = total + floor(sqrt(i))
    return total
