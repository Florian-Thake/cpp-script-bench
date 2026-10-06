# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

def bench(n):
    total = 0
    for i in range(n):
        total = total + (i & 7)
    return total
