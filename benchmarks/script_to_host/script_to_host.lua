-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

-- f0, f1 and f3 are C++ functions in the table 'host', which the host registers.
-- The idiomatic fast form, like the math functions in math_builtins: the functions in local
-- variables, which bench reaches as upvalues, instead of a lookup in the table host on every call.
local f0, f1, f3 = host.f0, host.f1, host.f3

-- bench is global, so the host can find it.
function bench(n)
    local sum = 0
    for i = 0, n - 1 do
        sum = sum + f0() + f1(i) + f3(i, 1, 2)
    end
    return sum
end
