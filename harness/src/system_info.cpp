/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include "system_info.hpp"

#include "csb_git_version.h"

#include <cstring>
#include <string>

#if defined( _WIN32 )
# include <windows.h>
# include <vector>
#else
# include <fstream>
# include <set>
# include <utility>
# include <sys/utsname.h>
# include <unistd.h>
#endif

#if defined( _MSC_VER ) && (defined( _M_X64 ) || defined( _M_IX86 ))
# include <intrin.h>
# define CSB_HAS_CPUID 1
#elif defined( __GNUC__ ) && (defined( __x86_64__ ) || defined( __i386__ ))
# include <cpuid.h>
# define CSB_HAS_CPUID 1
#else
# define CSB_HAS_CPUID 0
#endif


namespace csb {

namespace {

std::string trim( std::string const &s )
{
    char const *const ws = " \t\r\n";
    auto const first = s.find_first_not_of( ws );
    if( first == std::string::npos ) {
        return {};
    }
    auto const last = s.find_last_not_of( ws );
    return s.substr( first, last - first + 1 );
}

/// \return \param s without leading and trailing spaces, and with single spaces between its words.
std::string single_spaced( std::string const &s )
{
    std::string res;
    for( char const c : trim( s ) ) {
        if( c != ' ' || res.empty() || res.back() != ' ' ) {
            res += c;
        }
    }
    return res;
}

#if CSB_HAS_CPUID
void cpuid( unsigned int const leaf, unsigned int (&regs)[4] )
{
# if defined( _MSC_VER )
    int r[4] = {};
    __cpuid( r, static_cast<int>(leaf) );
    std::memcpy( regs, r, sizeof( regs ) );
# else
    __cpuid( leaf, regs[0], regs[1], regs[2], regs[3] );
# endif
}
#endif

std::string cpu_brand()
{
#if CSB_HAS_CPUID
    unsigned int regs[4] = {};
    cpuid( 0x80000000u, regs );
    if( regs[0] < 0x80000004u ) {
        return "unknown";
    }
    char brand[49] = {};
    for( unsigned int i = 0; i < 3; ++i ) {
        cpuid( 0x80000002u + i, regs );
        std::memcpy( brand + i * 16, regs, 16 );
    }
    return trim( brand );
#else
    return "unknown";
#endif
}

std::string hypervisor()
{
#if CSB_HAS_CPUID
    unsigned int regs[4] = {};
    cpuid( 1, regs );
    if( (regs[2] & (1u << 31)) == 0 ) {  // the hypervisor present bit
        return {};
    }
    cpuid( 0x40000000u, regs );
    auto const max_leaf = regs[0];
    char signature[13] = {};
    std::memcpy( signature, &regs[1], 4 );
    std::memcpy( signature + 4, &regs[2], 4 );
    std::memcpy( signature + 8, &regs[3], 4 );
    std::string name = trim( signature );
    if( name.empty() ) {
        return "unknown";
    }
    // Hyper-V: the root partition, which runs the host OS, may create partitions (bit 0 of EBX of CPUID 0x40000003).
    // With Hyper-V enabled (e.g. for WSL2), Windows itself runs in the root partition.
    if( name == "Microsoft Hv" && max_leaf >= 0x40000003u ) {
        cpuid( 0x40000003u, regs );
        if( (regs[1] & 1u) != 0 ) {
            name += " (root partition)";
        }
    }
    return name;
#else
    return "unknown";
#endif
}

#if defined( _WIN32 )

std::string utf8( wchar_t const *const s )
{
    int const size = ::WideCharToMultiByte( CP_UTF8, 0, s, -1, nullptr, 0, nullptr, nullptr );
    if( size <= 1 ) {
        return {};
    }
    std::string res( static_cast<size_t>(size), '\0' );
    ::WideCharToMultiByte( CP_UTF8, 0, s, -1, res.data(), size, nullptr, nullptr );
    res.resize( static_cast<size_t>(size - 1) );
    return res;
}

wchar_t const *const version_key = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion";

std::string registry_string( wchar_t const *const name )
{
    wchar_t buf[256] = {};
    DWORD   size     = sizeof( buf );
    if( ::RegGetValueW( HKEY_LOCAL_MACHINE, version_key, name, RRF_RT_REG_SZ, nullptr, buf, &size ) != ERROR_SUCCESS ) {
        return {};
    }
    return utf8( buf );
}

bool registry_dword( wchar_t const *const name, DWORD &value )
{
    DWORD size = sizeof( value );
    return ::RegGetValueW( HKEY_LOCAL_MACHINE, version_key, name, RRF_RT_REG_DWORD, nullptr, &value, &size ) == ERROR_SUCCESS;
}

/// \return the version of Windows, e.g. 10.0.19045. GetVersionEx() lies about it for not manifested applications,
/// RtlGetVersion() does not.
bool windows_version( RTL_OSVERSIONINFOW &info )
{
    using RtlGetVersionFunc = LONG (WINAPI *)(PRTL_OSVERSIONINFOW);
    auto const ntdll = ::GetModuleHandleW( L"ntdll.dll" );
    auto const func  = ntdll != nullptr ? reinterpret_cast<RtlGetVersionFunc>(::GetProcAddress( ntdll, "RtlGetVersion" )) : nullptr;
    info = {};
    info.dwOSVersionInfoSize = sizeof( info );
    return func != nullptr && func( &info ) == 0;
}

std::string os_kernel()
{
    RTL_OSVERSIONINFOW info;
    if( not windows_version( info ) ) {
        return "Windows";
    }
    std::string res = "Windows " + std::to_string( info.dwMajorVersion ) + "." + std::to_string( info.dwMinorVersion ) + "."
                    + std::to_string( info.dwBuildNumber );
    DWORD revision = 0;  // the update build revision
    if( registry_dword( L"UBR", revision ) ) {
        res += "." + std::to_string( revision );
    }
    return res;
}

std::string os_product()
{
    auto product = registry_string( L"ProductName" );
    if( product.empty() ) {
        return "Windows";
    }
    // Windows 11 still has "Windows 10" in its ProductName; it starts with build 22000.
    RTL_OSVERSIONINFOW info;
    if( windows_version( info ) && info.dwBuildNumber >= 22000 && product.starts_with( "Windows 10" ) ) {
        product.replace( 0, 10, "Windows 11" );
    }
    auto const display_version = registry_string( L"DisplayVersion" );  // e.g. 22H2
    return display_version.empty() ? product : product + " " + display_version;
}

int cpu_cores()
{
    DWORD size = 0;
    (void)::GetLogicalProcessorInformationEx( RelationProcessorCore, nullptr, &size );
    std::vector<char> buf( size );
    if( size == 0 || not ::GetLogicalProcessorInformationEx( RelationProcessorCore, reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buf.data()), &size ) ) {
        return 0;
    }
    int cores = 0;
    for( DWORD offset = 0; offset < size; ++cores ) {
        offset += reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buf.data() + offset)->Size;
    }
    return cores;
}

int cpu_threads()
{
    return static_cast<int>(::GetActiveProcessorCount( ALL_PROCESSOR_GROUPS ));
}

std::uint64_t physical_memory()
{
    MEMORYSTATUSEX status{};
    status.dwLength = sizeof( status );
    return ::GlobalMemoryStatusEx( &status ) ? status.ullTotalPhys : 0;
}

#else

std::string os_kernel()
{
    utsname info{};
    if( ::uname( &info ) != 0 ) {
        return "unknown";
    }
    return std::string( info.sysname ) + " " + info.release;
}

/// \return PRETTY_NAME of os-release, e.g. "Ubuntu 22.04.5 LTS".
std::string os_product()
{
    for( char const *const path : { "/etc/os-release", "/usr/lib/os-release" } ) {
        std::ifstream in( path );
        std::string   line;
        while( std::getline( in, line ) ) {
            if( line.starts_with( "PRETTY_NAME=" ) ) {
                auto value = trim( line.substr( 12 ) );
                if( value.size() >= 2 && (value.front() == '"' || value.front() == '\'') && value.back() == value.front() ) {
                    value = value.substr( 1, value.size() - 2 );
                }
                return value;
            }
        }
    }
    return "unknown";
}

int cpu_threads()
{
    return static_cast<int>(::sysconf( _SC_NPROCESSORS_ONLN ));
}

/// \return the number of distinct pairs of "physical id" and "core id" in /proc/cpuinfo, or the number of logical
/// processors if it has none.
int cpu_cores()
{
    std::ifstream in( "/proc/cpuinfo" );
    std::set<std::pair<std::string, std::string>> cores;
    std::string line, physical, core;
    auto const value = []( std::string const &l ) {
        auto const pos = l.find( ':' );
        return pos == std::string::npos ? std::string() : trim( l.substr( pos + 1 ) );
    };
    auto const add = [&]() {
        if( not core.empty() ) {
            cores.emplace( physical, core );
        }
        physical.clear();
        core.clear();
    };
    while( std::getline( in, line ) ) {
        if( line.starts_with( "physical id" ) ) {
            physical = value( line );
        } else if( line.starts_with( "core id" ) ) {
            core = value( line );
        } else if( trim( line ).empty() ) {
            add();
        }
    }
    add();
    return cores.empty() ? cpu_threads() : static_cast<int>(cores.size());
}

std::uint64_t physical_memory()
{
    auto const pages     = ::sysconf( _SC_PHYS_PAGES );
    auto const page_size = ::sysconf( _SC_PAGE_SIZE );
    return pages > 0 && page_size > 0 ? static_cast<std::uint64_t>(pages) * static_cast<std::uint64_t>(page_size) : 0;
}

#endif

std::string compiler_name()
{
#if defined( __clang__ )
    return "Clang " __clang_version__;
#elif defined( _MSC_VER )
    // _MSC_FULL_VER is e.g. 194435207 for 19.44.35207
    return "MSVC " + std::to_string( _MSC_FULL_VER / 10'000'000 ) + "." + std::to_string( (_MSC_FULL_VER / 100'000) % 100 )
         + "." + std::to_string( _MSC_FULL_VER % 100'000 );
#elif defined( __GNUC__ )
    return "GCC " __VERSION__;
#else
    return "unknown";
#endif
}

} // namespace


SystemInfo query_system_info()
{
    SystemInfo info;
    info.cpu            = cpu_brand();
    info.cpu_cores      = cpu_cores();
    info.cpu_threads    = cpu_threads();
    info.memory         = physical_memory();
    info.os             = os_kernel();
    info.os_name        = os_product();
    info.hypervisor     = hypervisor();
    info.compiler       = compiler_name();
    info.compiler_flags = single_spaced( CSB_COMPILER_FLAGS );
    info.build_config   = CSB_BUILD_CONFIG;
    info.arch           = CSB_ARCH;
    info.csb_version    = std::string( CSB_VERSION ) + " (" + CSB_GIT_VERSION + ")";
    return info;
}

} // namespace csb
