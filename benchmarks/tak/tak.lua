-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

-- 'local function' is the idiomatic form for a module-level function in Lua. The recursive calls
-- then reach tak through an upvalue instead of a lookup in the global table.
local function tak(x, y, z)
    if y < x then
        return tak(tak(x - 1, y, z), tak(y - 1, z, x), tak(z - 1, x, y))
    else
        return z
    end
end

-- bench is global, so the host can find it.
function bench(n)
    return tak(3 * n, 2 * n, n)
end
