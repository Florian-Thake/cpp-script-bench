# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

# The script-level variable: a module variable.
total = 0


def bench(n):
    global total
    total = 0
    for i in range(n):
        total = total + (i & 7)
    return total
