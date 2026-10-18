# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

def bench(n):
    total = 0
    for a in range(n):
        for b in range(n):
            for c in range(n):
                for d in range(n):
                    total = total + ((a + b + c + d) & 7)
    return total
