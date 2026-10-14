-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

-- The idiomatic fast form: the library functions in local variables, which bench reaches as
-- upvalues, instead of a lookup in the table math on every call.
local sqrt, floor = math.sqrt, math.floor

-- bench is global, so the host can find it.
function bench(n)
    local total = 0
    for i = 1, n do
        total = total + floor(sqrt(i))
    end
    return total
end
