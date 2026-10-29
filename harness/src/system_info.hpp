/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <cstdint>
#include <string>


namespace csb {

/// The environment of a benchmark run. Recorded in every result for reproducibility.
/// In a virtual machine, the CPU topology and the memory are those of the VM.
struct SystemInfo
{
    std::string    cpu;
    int            cpu_cores   = 0;  ///< physical cores
    int            cpu_threads = 0;  ///< logical processors
    std::uint64_t  memory      = 0;  ///< physical memory in bytes
    std::string    os;               ///< the kernel, e.g. "Windows 10.0.19045.6456" or "Linux 5.15.167.4-microsoft-standard-WSL2"
    std::string    os_name;          ///< the product or distribution, e.g. "Windows 10 Pro 22H2" or "Ubuntu 22.04.5 LTS"
    std::string    hypervisor;       ///< the CPUID signature of the hypervisor, e.g. "KVMKVMKVM"; empty if there is none
    std::string    compiler;
    std::string    compiler_flags;   ///< the C++ flags of the build configuration and of CSB_ARCH
    std::string    build_config;     ///< e.g. "Release"
    std::string    arch;             ///< CSB_ARCH, e.g. "x86-64-v3"
    std::string    csb_version;      ///< version and git info of cpp-script-bench itself
};

SystemInfo query_system_info();

} // namespace csb
