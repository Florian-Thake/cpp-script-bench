-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

function bench(n)
    local max_iter <const> = 50
    local count = 0
    for y = 0, n - 1 do
        local ci = -1.25 + 2.5 * y / n
        for x = 0, n - 1 do
            local cr = -2.0 + 2.5 * x / n
            local zr = 0.0
            local zi = 0.0
            local k = 0
            while k < max_iter and zr * zr + zi * zi <= 4.0 do
                local t = zr * zr - zi * zi + cr
                zi = 2.0 * zr * zi + ci
                zr = t
                k = k + 1
            end
            count = count + k
        end
    end
    return count
end
