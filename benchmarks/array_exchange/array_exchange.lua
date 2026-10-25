-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

-- The host calls f(a) in its loop with a sequence of integers and converts the result back (there
-- is no bench). f returns a new sequence with every element + 1, built by appending.
-- f is global, so the host can find it.
function f(a)
    local size = #a
    local b = table.create(size)
    for i = 1, size do
        b[i] = a[i] + 1
    end
    return b
end
