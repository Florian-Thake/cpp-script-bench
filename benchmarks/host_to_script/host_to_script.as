// SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
// SPDX-License-Identifier: MIT

// The functions of 'calls', which the host calls in its loop (there is no bench).

int f0()
{
    return 1;
}

int f1( int a )
{
    return a & 7;
}

int f3( int a, int b, int c )
{
    return (a + b + c) & 7;
}
