-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

-- The script of engine_reset: a top-level variable and two functions. The runner loads it, resets
-- the engine and loads it again (see README.md).
-- offset and bench are globals of the environment of the script, which the reset replaces.

offset = 3

local function f1(a)
    return a & 7
end

function bench(n)
    local sum = offset
    for i = 0, n - 1 do
        sum = sum + f1(i)
    end
    return sum
end
