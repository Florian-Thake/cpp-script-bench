-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

-- A node is a table with the fields left and right; a leaf has none (nil).
local function make(d)
    if d == 0 then
        return { left = nil, right = nil }
    end
    return { left = make(d - 1), right = make(d - 1) }
end

local function check(t)
    if t.left == nil then
        return 1
    end
    return 1 + check(t.left) + check(t.right)
end

-- bench is global, so the host can find it.
function bench(n)
    local sum = 0
    local count = 1 << (16 - n)
    for k = 1, count do
        local t = make(n)
        sum = sum + check(t)
    end
    return sum
end
