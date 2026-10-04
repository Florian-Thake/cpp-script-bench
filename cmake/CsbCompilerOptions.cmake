# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

# Options applied to every target built from source (harness, runners and later also script
# libraries like Lua or AngelScript), so all languages are compiled with the same settings.
# The optimization flags come from the build configuration, see CMakePresets.json.
function( csb_apply_compiler_options target )
    if( MSVC )
        target_compile_options( ${target} PRIVATE
            $<$<COMPILE_LANGUAGE:CXX>:/permissive- /Zc:__cplusplus /EHsc>
            /utf-8 /bigobj /W3 )
        if( CSB_ARCH STREQUAL "x86-64-v3" )
            target_compile_options( ${target} PRIVATE /arch:AVX2 )
        endif()
        target_compile_definitions( ${target} PRIVATE _CRT_SECURE_NO_WARNINGS NOMINMAX WIN32_LEAN_AND_MEAN )
    else()
        target_compile_options( ${target} PRIVATE -Wall -Wextra )
        if( CSB_ARCH STREQUAL "x86-64-v3" )
            target_compile_options( ${target} PRIVATE -march=x86-64-v3 )
        endif()
    endif()
endfunction()
