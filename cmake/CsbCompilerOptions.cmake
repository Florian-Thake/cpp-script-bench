# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

# Sets <out_var> to the compiler flag for CSB_ARCH, or to "" for the compiler default.
function( csb_arch_flag out_var )
    set( flag "" )
    if( CSB_ARCH STREQUAL "x86-64-v3" )
        if( MSVC )
            set( flag /arch:AVX2 )
        else()
            set( flag -march=x86-64-v3 )
        endif()
    endif()
    set( ${out_var} "${flag}" PARENT_SCOPE )
endfunction()

# Options applied to every target built from source (harness, runners and script libraries like
# Lua or AngelScript), so all languages are compiled with the same settings.
# The optimization flags come from the build configuration, see CMakePresets.json.
# The options apply to C and C++ sources only (not to assembler sources like AngelScript's MASM file).
function( csb_apply_compiler_options target )
    csb_arch_flag( arch )
    if( MSVC )
        target_compile_options( ${target} PRIVATE
            $<$<COMPILE_LANGUAGE:CXX>:/permissive- /Zc:__cplusplus /EHsc>
            $<$<COMPILE_LANGUAGE:C,CXX>:/utf-8 /bigobj /W3> )
        target_compile_definitions( ${target} PRIVATE _CRT_SECURE_NO_WARNINGS NOMINMAX WIN32_LEAN_AND_MEAN )
    else()
        target_compile_options( ${target} PRIVATE $<$<COMPILE_LANGUAGE:C,CXX>:-Wall -Wextra> )
    endif()
    if( arch )
        target_compile_options( ${target} PRIVATE $<$<COMPILE_LANGUAGE:C,CXX>:${arch}> )
    endif()
endfunction()

# Sets <out_var> to the C++ flags of the build configuration and of CSB_ARCH, for the result files. It is a generator
# expression, because the configuration of a multi-config generator (Visual Studio) is only known at build time.
function( csb_compiler_flags out_var )
    csb_arch_flag( arch )
    set( config "" )
    foreach( name Debug Release RelWithDebInfo MinSizeRel )
        string( TOUPPER ${name} upper )
        string( APPEND config "$<$<CONFIG:${name}>:${CMAKE_CXX_FLAGS_${upper}}>" )
    endforeach()
    string( STRIP "${CMAKE_CXX_FLAGS} ${config} ${arch}" flags )
    set( ${out_var} "${flags}" PARENT_SCOPE )
endfunction()
