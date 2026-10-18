# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

# The record type.
class Point:
    def __init__(self):
        self.x = 0
        self.y = 0
        self.z = 0


def bench(n):
    p = Point()
    for i in range(n):
        p.x = p.x + (i & 7)
        p.y = p.y + (p.x & 3)
        p.z = p.z + (p.y & 1)
    return p.x + p.y + p.z
