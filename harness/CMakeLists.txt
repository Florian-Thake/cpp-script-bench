# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

# The harness is shared by all runner executables: command line, measurement loop,
# result verification, statistics and reporting.
add_library( csb_harness STATIC
    src/json_report.cpp
    src/runner.cpp
    src/stats.cpp
    src/system_info.cpp )

target_include_directories( csb_harness
    PUBLIC  include
    PRIVATE src )

csb_compiler_flags( csb_flags )
target_compile_definitions( csb_harness PRIVATE
    CSB_VERSION="${PROJECT_VERSION}"
    CSB_ARCH="${CSB_ARCH}"
    CSB_BUILD_CONFIG="$<CONFIG>"
    CSB_COMPILER_FLAGS="${csb_flags}"
    CSB_DEFAULT_SCRIPTS_DIR="${PROJECT_SOURCE_DIR}/benchmarks" )

csb_target_git_version( csb_harness CSB_GIT_VERSION "${PROJECT_SOURCE_DIR}" )
csb_apply_compiler_options( csb_harness )

if( WIN32 )
    target_link_libraries( csb_harness PRIVATE advapi32 )  # the registry, for the name of the OS
endif()
