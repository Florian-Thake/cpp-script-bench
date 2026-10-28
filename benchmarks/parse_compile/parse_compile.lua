-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

-- A template: the runner generates the marked function n times, as fn0 .. fn<n-1> (see README.md).
-- The helper is a 'local function' like in the other scripts. The generated functions are global:
-- 'local function' would exceed Lua's limit of 200 local variables per function.

local function g(x)
    return 2 * x + 1
end

-- csb:repeat
function fn__I__(a)
    local sum = 0
    for i = 0, a - 1 do
        local r = i % 3
        if r == 0 then
            sum = sum + g(i)
        else
            sum = sum - i * r
        end
    end
    return sum + __I__
end

-- csb:end
function bench(n)
    return fn0(n) + fn__LAST__(n)
end
