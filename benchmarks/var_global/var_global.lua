-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

-- The script-level variable: a global (no 'local'), i.e. a field of the global environment.
sum = 0

function bench(n)
    sum = 0
    for i = 0, n - 1 do
        sum = sum + (i & 7)
    end
    return sum
end
