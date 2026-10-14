-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

function bench(n)
    local total = 0
    for s = 1, n do
        local x = s
        while x ~= 1 do
            if x % 2 == 0 then
                x = x // 2
            else
                x = 3 * x + 1
            end
            total = total + 1
        end
    end
    return total
end
