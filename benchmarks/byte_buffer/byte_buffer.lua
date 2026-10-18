-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

-- Lua has no byte type: the buffer is a table of integers in 0..255, built by appending.
-- Lua indexes from 1: the byte with the index i of the spec has the index i + 1.
function bench(n)
    local buf = table.create(n)
    local x = 1
    for i = 1, n do
        x = (x * 75 + 74) % 65537
        buf[i] = x & 255
    end
    local a = 1
    local b = 0
    for i = 1, n do
        a = (a + buf[i]) % 65521
        b = (b + a) % 65521
    end
    return a + b
end
