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
#else
# include <sys/utsname.h>
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
    auto const first = s.find_first_not_of( ' ' );
    if( first == std::string::npos ) {
        return {};
    }
    auto const last = s.find_last_not_of( ' ' );
    return s.substr( first, last - first + 1 );
}

std::string cpu_brand()
{
#if CSB_HAS_CPUID
    unsigned int regs[4] = {};
    auto const cpuid = [&regs]( unsigned int const leaf ) {
# if defined( _MSC_VER )
        int r[4] = {};
        __cpuid( r, static_cast<int>(leaf) );
        std::memcpy( regs, r, sizeof( regs ) );
# else
        __cpuid( leaf, regs[0], regs[1], regs[2], regs[3] );
# endif
    };

    cpuid( 0x80000000u );
    if( regs[0] < 0x80000004u ) {
        return "unknown";
    }
    char brand[49] = {};
    for( unsigned int i = 0; i < 3; ++i ) {
        cpuid( 0x80000002u + i );
        std::memcpy( brand + i * 16, regs, 16 );
    }
    return trim( brand );
#else
    return "unknown";
#endif
}

std::string os_name()
{
#if defined( _WIN32 )
    // GetVersionEx() lies about the version for not manifested applications, RtlGetVersion() does not.
    using RtlGetVersionFunc = LONG (WINAPI *)(PRTL_OSVERSIONINFOW);
    auto const ntdll = ::GetModuleHandleW( L"ntdll.dll" );
    auto const func  = ntdll != nullptr ? reinterpret_cast<RtlGetVersionFunc>(::GetProcAddress( ntdll, "RtlGetVersion" )) : nullptr;
    RTL_OSVERSIONINFOW info{};
    info.dwOSVersionInfoSize = sizeof( info );
    if( func == nullptr || func( &info ) != 0 ) {
        return "Windows";
    }
    return "Windows " + std::to_string( info.dwMajorVersion ) + "." + std::to_string( info.dwMinorVersion ) + "." + std::to_string( info.dwBuildNumber );
#else
    utsname info{};
    if( ::uname( &info ) != 0 ) {
        return "unknown";
    }
    return std::string( info.sysname ) + " " + info.release;
#endif
}

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
    info.cpu          = cpu_brand();
    info.os           = os_name();
    info.compiler     = compiler_name();
    info.build_config = CSB_BUILD_CONFIG;
    info.arch         = CSB_ARCH;
    info.csb_version  = std::string( CSB_VERSION ) + " (" + CSB_GIT_VERSION + ")";
    return info;
}

} // namespace csb
