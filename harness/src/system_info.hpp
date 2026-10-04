/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <string>


namespace csb {

/// The environment of a benchmark run. Recorded in every result for reproducibility.
struct SystemInfo
{
    std::string  cpu;
    std::string  os;
    std::string  compiler;
    std::string  build_config;  ///< e.g. "Release"
    std::string  arch;          ///< CSB_ARCH, e.g. "x86-64-v3"
    std::string  csb_version;   ///< version and git info of cpp-script-bench itself
};

SystemInfo query_system_info();

} // namespace csb
