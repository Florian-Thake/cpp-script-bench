-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

-- 'local function' is the idiomatic form for a module-level function in Lua. bench then reaches
-- the functions through upvalues instead of lookups in the global table.
local function f0()
    return 1
end

local function f1(a)
    return a & 7
end

local function f3(a, b, c)
    return (a + b + c) & 7
end

-- bench is global, so the host can find it.
function bench(n)
    local sum = 0
    for i = 0, n - 1 do
        sum = sum + f0() + f1(i) + f3(i, 1, 2)
    end
    return sum
end
