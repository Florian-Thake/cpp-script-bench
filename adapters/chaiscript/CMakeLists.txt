# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

if( NOT EXISTS "${CSB_CHAISCRIPT_DIR}/include/chaiscript/chaiscript.hpp" )
    message( FATAL_ERROR "CSB_WITH_CHAISCRIPT is ON, but CSB_CHAISCRIPT_DIR ('${CSB_CHAISCRIPT_DIR}') does not contain include/chaiscript/chaiscript.hpp" )
endif()

csb_git_describe( "${CSB_CHAISCRIPT_DIR}" CSB_CHAISCRIPT_GIT_VERSION )
message( STATUS "ChaiScript runners: ${CSB_CHAISCRIPT_DIR} (${CSB_CHAISCRIPT_GIT_VERSION})" )

find_package( Threads REQUIRED )

# The same source is built twice: in ChaiScript's default (thread-safe) configuration and with CHAISCRIPT_NO_THREADS.
function( csb_add_chaiscript_runner target no_threads )
    add_executable( ${target} main.cpp )
    target_include_directories( ${target} SYSTEM PRIVATE "${CSB_CHAISCRIPT_DIR}/include" )
    target_compile_definitions( ${target} PRIVATE
        CHAISCRIPT_NO_DYNLOAD )  # loading binary modules is disabled anyway (no IO)
    if( no_threads )
        target_compile_definitions( ${target} PRIVATE CHAISCRIPT_NO_THREADS )
    endif()
    target_link_libraries( ${target} PRIVATE csb_benchmarks Threads::Threads )
    csb_target_git_version( ${target} CSB_CHAISCRIPT_GIT_VERSION "${CSB_CHAISCRIPT_DIR}" )
    csb_apply_compiler_options( ${target} )
endfunction()

csb_add_chaiscript_runner( csb_chaiscript OFF )
csb_add_chaiscript_runner( csb_chaiscript_nothreads ON )
