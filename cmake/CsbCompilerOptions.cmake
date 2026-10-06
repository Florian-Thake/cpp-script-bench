# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

# Options applied to every target built from source (harness, runners and script libraries like
# Lua or AngelScript), so all languages are compiled with the same settings.
# The optimization flags come from the build configuration, see CMakePresets.json.
# The options apply to C and C++ sources only (not to assembler sources like AngelScript's MASM file).
function( csb_apply_compiler_options target )
    if( MSVC )
        target_compile_options( ${target} PRIVATE
            $<$<COMPILE_LANGUAGE:CXX>:/permissive- /Zc:__cplusplus /EHsc>
            $<$<COMPILE_LANGUAGE:C,CXX>:/utf-8 /bigobj /W3> )
        if( CSB_ARCH STREQUAL "x86-64-v3" )
            target_compile_options( ${target} PRIVATE $<$<COMPILE_LANGUAGE:C,CXX>:/arch:AVX2> )
        endif()
        target_compile_definitions( ${target} PRIVATE _CRT_SECURE_NO_WARNINGS NOMINMAX WIN32_LEAN_AND_MEAN )
    else()
        target_compile_options( ${target} PRIVATE $<$<COMPILE_LANGUAGE:C,CXX>:-Wall -Wextra> )
        if( CSB_ARCH STREQUAL "x86-64-v3" )
            target_compile_options( ${target} PRIVATE $<$<COMPILE_LANGUAGE:C,CXX>:-march=x86-64-v3> )
        endif()
    endif()
endfunction()
