# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

def bench(n):
    a = []
    x = 1
    for _ in range(n):
        x = (x * 75 + 74) % 65537
        a.append(x)
    for i in range(1, n):
        v = a[i]
        j = i - 1
        while j >= 0 and a[j] > v:
            a[j + 1] = a[j]
            j = j - 1
        a[j + 1] = v
    total = 0
    for i in range(n):
        total = total + a[i] * (i & 7)
    return total
