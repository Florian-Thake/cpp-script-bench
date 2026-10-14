# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

def bench(n):
    total = 0
    for i in range(n):
        k = i & 7
        if k == 0:
            total = total + 1
        elif k == 1:
            total = total + 2
        elif k < 4:
            total = total + 3
        else:
            total = total + 4
        if (k > 2 and k < 6) or k == 7:
            total = total + 5
    return total
