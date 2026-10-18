-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

function bench(n)
    -- The record: a table with named fields.
    local p = { x = 0, y = 0, z = 0 }
    for i = 0, n - 1 do
        p.x = p.x + (i & 7)
        p.y = p.y + (p.x & 3)
        p.z = p.z + (p.y & 1)
    end
    return p.x + p.y + p.z
end
