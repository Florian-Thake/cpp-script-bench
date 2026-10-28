-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

-- The check after the registration, not measured: bench( n ) calls the first and the last
-- of the n functions registered by the host (the runner replaces __LAST__ with n-1, see README.md).

function bench(n)
    return host.fn0(n) + host.fn__LAST__(n + 3)
end
