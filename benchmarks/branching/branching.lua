-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

function bench(n)
    local sum = 0
    for i = 0, n - 1 do
        local k = i & 7
        if k == 0 then
            sum = sum + 1
        elseif k == 1 then
            sum = sum + 2
        elseif k < 4 then
            sum = sum + 3
        else
            sum = sum + 4
        end
        if (k > 2 and k < 6) or k == 7 then
            sum = sum + 5
        end
    end
    return sum
end
