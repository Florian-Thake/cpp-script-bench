# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

cmake_minimum_required( VERSION 3.22 )

project( cpp-script-bench
    VERSION 0.1.0
    DESCRIPTION "Fair benchmarks of script languages embedded in C++"
    LANGUAGES CXX )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )
include( CsbCompilerOptions )
include( CsbGitInfo )


# === common settings ===

set( CSB_ARCH "x86-64-v3" CACHE STRING "Instruction set for all code built from source: x86-64-v3 (AVX2) or default (compiler default)" )
set_property( CACHE CSB_ARCH PROPERTY STRINGS x86-64-v3 default )

# TeaScript uses the fmt library (header-only) if found, otherwise std::format.
# Required for compilers without std::format, e.g. GCC < 13.
set( CSB_FMT_DIR "" CACHE PATH "Optional: root of the fmt library (the directory containing include/fmt)" )


# === languages ===
# The script language libraries are provided by the user, see README.md (CMakeUserPresets.json).

option( CSB_WITH_TEASCRIPT "Build the TeaScript runner (needs CSB_TEASCRIPT_DIR)" OFF )
set( CSB_TEASCRIPT_DIR "" CACHE PATH "Root of the TeaScript C++ Library (the directory containing include/teascript)" )
set( CSB_TEASCRIPT_DEFAULT_VARIANT "vm-O1" CACHE STRING "TeaScript variant used when no --variant is given" )
set_property( CACHE CSB_TEASCRIPT_DEFAULT_VARIANT PROPERTY STRINGS vm-O0 vm-O1 vm-O2 ast )

option( CSB_WITH_CHAISCRIPT "Build the ChaiScript runners (needs CSB_CHAISCRIPT_DIR and CSB_CHAISCRIPT_EXTRAS_DIR)" OFF )
set( CSB_CHAISCRIPT_DIR "" CACHE PATH "Root of ChaiScript (the directory containing include/chaiscript); use the develop branch" )
set( CSB_CHAISCRIPT_EXTRAS_DIR "" CACHE PATH "Root of ChaiScript_Extras (the directory containing include/chaiscript/extras), for its math module" )

option( CSB_WITH_LUA "Build the Lua runner (needs CSB_LUA_DIR)" OFF )
set( CSB_LUA_DIR "" CACHE PATH "Root of the Lua source distribution (the directory containing src/lua.h)" )

option( CSB_WITH_ANGELSCRIPT "Build the AngelScript runners (needs CSB_ANGELSCRIPT_DIR)" OFF )
set( CSB_ANGELSCRIPT_DIR "" CACHE PATH "Root of the AngelScript SDK (the directory containing angelscript/include and add_on)" )

# Python is the installed one, found by CMake (FindPython). Another installed Python can be chosen with the
# standard hints Python_ROOT_DIR or Python_EXECUTABLE.
option( CSB_WITH_PYTHON "Build the Python runner (needs CSB_PYBIND11_DIR and an installed Python with its development files)" OFF )
set( CSB_PYBIND11_DIR "" CACHE PATH "Root of pybind11 (the directory containing include/pybind11)" )

# Lua is written in C. AngelScript needs an assembler file for native calls with MSVC x64.
if( CSB_WITH_LUA )
    enable_language( C )
endif()
if( CSB_WITH_ANGELSCRIPT AND MSVC )
    enable_language( ASM_MASM )
endif()


csb_git_describe( "${CMAKE_CURRENT_SOURCE_DIR}" CSB_GIT_VERSION )
message( STATUS "cpp-script-bench ${PROJECT_VERSION} (${CSB_GIT_VERSION}), arch: ${CSB_ARCH}" )

add_subdirectory( harness )
add_subdirectory( benchmarks )
add_subdirectory( adapters )
