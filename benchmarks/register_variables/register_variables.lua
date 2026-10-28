-- SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
-- SPDX-License-Identifier: MIT

-- The check after the registration, not measured: bench( n ) adds the first and the last
-- of the n variables registered by the host (the runner replaces __LAST__ with n-1, see README.md).

function bench(n)
    return host.v0 + host.v__LAST__
end
