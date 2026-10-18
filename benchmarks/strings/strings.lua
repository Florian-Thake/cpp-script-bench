-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

-- The idiomatic fast form: the library functions in local variables, which bench reaches as
-- upvalues, instead of a lookup in the global table (tonumber) or in the table string (sub).
local sub, tonumber = string.sub, tonumber

-- bench is global, so the host can find it.
function bench(n)
    local sum = 0
    local prev = ""
    for i = 0, n - 1 do
        local s = "item" .. i
        if s < prev then
            sum = sum + 1
        end
        sum = sum + #s
        local d = sub(s, 5)
        sum = sum + (tonumber(d) & 7)
        prev = s
    end
    return sum
end
