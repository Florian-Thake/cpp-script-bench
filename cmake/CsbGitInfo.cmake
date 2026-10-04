# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

find_package( Git QUIET )

# Sets <out_var> to the output of 'git describe --tags --always --dirty' for <dir>, or to "unknown".
# The result is recorded in every result file for reproducibility.
# Note: This runs at configure time. Re-run CMake after moving a library to another commit.
function( csb_git_describe dir out_var )
    set( result "unknown" )
    if( GIT_FOUND AND EXISTS "${dir}/.git" )
        execute_process(
            COMMAND "${GIT_EXECUTABLE}" -C "${dir}" describe --tags --always --dirty
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
