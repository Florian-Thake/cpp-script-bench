# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

# A template: the runner generates the marked function n times, as fn0 .. fn<n-1> (see README.md).
# There is no bench: the host calls the functions in its loop, through handles looked up once.

# csb:repeat
def fn__I__(a):
    return a + __I__


# csb:end
