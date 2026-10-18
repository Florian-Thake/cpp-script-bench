# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

def bench(n):
    flags = []
    for _ in range(n + 1):
        flags.append(True)
    i = 2
    while i * i <= n:
        if flags[i]:
            for j in range(i * i, n + 1, i):
                flags[j] = False
        i = i + 1
    count = 0
    for k in range(2, n + 1):
        if flags[k]:
            count = count + 1
    return count
