-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

-- Lua indexes from 1: the key with the index i of the spec ("k" .. i) has the index i + 1.
function bench(n)
    local keys = table.create(n)
    for i = 1, n do
        keys[i] = "k" .. (i - 1)
    end
    local m = table.create(0, n)
    for i = 1, n do
        m[keys[i]] = i - 1
    end
    local sum = 0
    for i = 1, n do
        sum = sum + m[keys[i]]
    end
    for i = 1, n, 2 do
        m[keys[i]] = nil
    end
    for i = 1, n do
        if m[keys[i]] ~= nil then
            sum = sum + 1
        end
    end
    return sum
end
