-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

function bench(n)
    local sum = 0
    for i = 0, n - 1 do
        sum = sum + (i & 7)
    end
    return sum
end
