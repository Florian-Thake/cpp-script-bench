-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

-- 'local function' is the idiomatic form for a module-level function in Lua. The recursive calls
-- then reach fib through an upvalue instead of a lookup in the global table.
local function fib(x)
    if x < 2 then
        return x
    else
        return fib(x - 1) + fib(x - 2)
    end
end

-- bench is global, so the host can find it.
function bench(n)
    return fib(n)
end
