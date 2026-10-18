# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

def bench(n):
    total = 0
    for i in range(n):
        a = i & 7
        b = a + 1
        b = b * a
        total = total + b
    return total
