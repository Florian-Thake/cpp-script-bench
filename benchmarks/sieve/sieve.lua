-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

-- Lua indexes from 1: the flag of the number m has the index m + 1.
function bench(n)
    local flags = table.create(n + 1)
    for k = 1, n + 1 do
        flags[k] = true
    end
    local i = 2
    while i * i <= n do
        if flags[i + 1] then
            for j = i * i + 1, n + 1, i do
                flags[j] = false
            end
        end
        i = i + 1
    end
    local count = 0
    for k = 3, n + 1 do
        if flags[k] then
            count = count + 1
        end
    end
    return count
end
