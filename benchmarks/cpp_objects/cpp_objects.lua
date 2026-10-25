-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

-- Vec3i is a C++ type in the table 'host', which the host binds: a constructor and the fields
-- x, y and z. The constructor in a local variable, like the functions in script_to_host.
local Vec3i = host.Vec3i

-- bench is global, so the host can find it.
function bench(n)
    local p = Vec3i()
    for i = 0, n - 1 do
        p.x = p.x + (i & 7)
        p.y = p.y + (p.x & 3)
        p.z = p.z + (p.y & 1)
    end
    return p.x + p.y + p.z
end
