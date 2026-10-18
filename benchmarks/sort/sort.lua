-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

-- Lua indexes from 1: the element with the index i of the spec has the index i + 1.
function bench(n)
    local a = table.create(n)
    local x = 1
    for k = 1, n do
        x = (x * 75 + 74) % 65537
        a[k] = x
    end
    for i = 2, n do
        local v = a[i]
        local j = i - 1
        while j >= 1 and a[j] > v do
            a[j + 1] = a[j]
            j = j - 1
        end
        a[j + 1] = v
    end
    local sum = 0
    for i = 0, n - 1 do
        sum = sum + a[i + 1] * (i & 7)
    end
    return sum
end
