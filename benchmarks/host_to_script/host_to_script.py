# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

# The functions of 'calls', which the host calls in its loop (there is no bench).

def f0():
    return 1


def f1(a):
    return a & 7


def f3(a, b, c):
    return (a + b + c) & 7
