# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

# The host calls f(a) in its loop with a list of integers and converts the result back (there
# is no bench). f returns a new list with every element + 1, built by appending.

def f(a):
    b = []
    for i in range(len(a)):
        b.append(a[i] + 1)
    return b
