-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

-- The host calls f(s) in its loop with a string and converts the result back (there is no bench).
-- f is global, so the host can find it.
function f(s)
    return s
end
