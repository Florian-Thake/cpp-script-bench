-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

-- host.image is a userdata of the host with n * n pixels, all 0; image:set(index, value) writes a
-- pixel, the index runs from 1 like in a Lua array. In a local variable, like the functions in
-- script_to_host.
local image = host.image

-- bench is global, so the host can find it.
function bench(n)
    for y = 0, n - 1 do
        for x = 0, n - 1 do
            image:set(y * n + x + 1, (x & 255) + (y & 255) * 256 + ((x + y) & 255) * 65536)
        end
    end
    return n * n
end
