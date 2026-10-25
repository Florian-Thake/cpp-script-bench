# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

# Vec3i is a C++ type in the module 'host', which the host binds: a constructor and the fields
# x, y and z.
from host import Vec3i


def bench(n):
    p = Vec3i()
    for i in range(n):
        p.x = p.x + (i & 7)
        p.y = p.y + (p.x & 3)
        p.z = p.z + (p.y & 1)
    return p.x + p.y + p.z
