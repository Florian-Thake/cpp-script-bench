# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

# The node type. A leaf has None as its children.
class Node:
    def __init__(self, left, right):
        self.left = left
        self.right = right


def make(d):
    if d == 0:
        return Node(None, None)
    return Node(make(d - 1), make(d - 1))


def check(t):
    if t.left is None:
        return 1
    return 1 + check(t.left) + check(t.right)


def bench(n):
    total = 0
    count = 1 << (16 - n)
    for _ in range(count):
        t = make(n)
        total = total + check(t)
    return total
