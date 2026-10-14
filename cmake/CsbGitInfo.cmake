# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

# In script mode (CsbGitVersionHeader.cmake) git is passed in as GIT_EXECUTABLE.
if( NOT GIT_EXECUTABLE )
    find_package( Git QUIET )
endif()

# Sets <out_var> to the output of 'git describe --tags --always --dirty' for <dir>, or to "unknown".
# core.autocrlf=input: a checkout made on Windows (CRLF) is not reported as dirty when built in WSL.
function( csb_git_describe dir out_var )
    set( result "unknown" )
    if( GIT_EXECUTABLE AND EXISTS "${dir}/.git" )
        execute_process(
            COMMAND "${GIT_EXECUTABLE}" -c core.autocrlf=input -C "${dir}" describe --tags --always --dirty
            OUTPUT_VARIABLE out
            RESULT_VARIABLE rc
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET )
        if( rc EQUAL 0 )
            set( result "${out}" )
        endif()
    endif()
    set( ${out_var} "${result}" PARENT_SCOPE )
endfunction()

# Provides csb_git_describe() of <dir> to <target> as the string macro <macro>, defined in the generated header
# '<macro in lower case>.h'. The result files record it for reproducibility.
# The header is checked on every build and only rewritten when the value changed. So it always names the commit which
# was built, also when <dir> moved to another commit without a re-run of CMake, and nothing is recompiled otherwise.
function( csb_target_git_version target macro dir )
    string( TOLOWER "${macro}" name )
    set( header "${CMAKE_CURRENT_BINARY_DIR}/${target}_git/${name}.h" )
    # One custom target per header, so a target can have several (e.g. a language library and an add-on library).
    add_custom_target( ${target}_${name}
        COMMAND "${CMAKE_COMMAND}" "-DGIT_EXECUTABLE=${GIT_EXECUTABLE}" "-DCSB_GIT_DIR=${dir}" "-DCSB_GIT_MACRO=${macro}"
                "-DCSB_GIT_HEADER=${header}" -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/CsbGitVersionHeader.cmake"
        BYPRODUCTS "${header}"  # Ninja: dependent sources are recompiled in the same build if the header changed
        VERBATIM )
    add_dependencies( ${target} ${target}_${name} )
    target_include_directories( ${target} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/${target}_git" )
endfunction()
