# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

def bench(n):
    keys = []
    for i in range(n):
        keys.append(f"k{i}")
    m = {}
    for i in range(n):
        m[keys[i]] = i
    total = 0
    for i in range(n):
        total = total + m[keys[i]]
    for i in range(0, n, 2):
        del m[keys[i]]
    for i in range(n):
        if keys[i] in m:
            total = total + 1
    return total
