# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

def bench(n):
    total = 0
    prev = ""
    for i in range(n):
        s = f"item{i}"
        if s < prev:
            total = total + 1
        total = total + len(s)
        d = s[4:]
        total = total + (int(d) & 7)
        prev = s
    return total
