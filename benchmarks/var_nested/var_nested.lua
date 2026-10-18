-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

function bench(n)
    local sum = 0
    for a = 0, n - 1 do
        for b = 0, n - 1 do
            for c = 0, n - 1 do
                for d = 0, n - 1 do
                    sum = sum + ((a + b + c + d) & 7)
                end
            end
        end
    end
    return sum
end
