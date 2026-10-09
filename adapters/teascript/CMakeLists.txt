# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

if( NOT EXISTS "${CSB_TEASCRIPT_DIR}/include/teascript/Engine.hpp" )
    message( FATAL_ERROR "CSB_WITH_TEASCRIPT is ON, but CSB_TEASCRIPT_DIR ('${CSB_TEASCRIPT_DIR}') does not contain include/teascript/Engine.hpp" )
endif()

csb_git_describe( "${CSB_TEASCRIPT_DIR}" CSB_TEASCRIPT_GIT_VERSION )
message( STATUS "TeaScript runner: ${CSB_TEASCRIPT_DIR} (${CSB_TEASCRIPT_GIT_VERSION}), default variant ${CSB_TEASCRIPT_DEFAULT_VARIANT}" )

add_executable( csb_teascript main.cpp )
target_include_directories( csb_teascript SYSTEM PRIVATE "${CSB_TEASCRIPT_DIR}/include" )
if( CSB_FMT_DIR )
    if( NOT EXISTS "${CSB_FMT_DIR}/include/fmt/format.h" )
        message( FATAL_ERROR "CSB_FMT_DIR ('${CSB_FMT_DIR}') does not contain include/fmt/format.h" )
    endif()
    target_include_directories( csb_teascript SYSTEM PRIVATE "${CSB_FMT_DIR}/include" )
    message( STATUS "TeaScript runner: using fmt from ${CSB_FMT_DIR}" )
endif()
target_compile_definitions( csb_teascript PRIVATE
    CSB_TEASCRIPT_DEFAULT_VARIANT="${CSB_TEASCRIPT_DEFAULT_VARIANT}" )
if( MSVC )
    target_compile_definitions( csb_teascript PRIVATE
        _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
        _SILENCE_CXX20_U8PATH_DEPRECATION_WARNING )
endif()
target_link_libraries( csb_teascript PRIVATE csb_benchmarks )
csb_target_git_version( csb_teascript CSB_TEASCRIPT_GIT_VERSION "${CSB_TEASCRIPT_DIR}" )
csb_apply_compiler_options( csb_teascript )
