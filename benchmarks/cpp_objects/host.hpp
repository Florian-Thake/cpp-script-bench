/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#pragma once


// The C++ type which every runner binds for the scripts of cpp_objects. The C++ reference uses it directly.
namespace csb::host::cpp_objects {

struct Vec3i
{
    int x = 0;
    int y = 0;
    int z = 0;
};

} // namespace csb::host::cpp_objects
