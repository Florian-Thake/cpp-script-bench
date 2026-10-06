# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

if( NOT EXISTS "${CSB_PYBIND11_DIR}/include/pybind11/embed.h" )
    message( FATAL_ERROR "CSB_WITH_PYTHON is ON, but CSB_PYBIND11_DIR ('${CSB_PYBIND11_DIR}') does not contain include/pybind11/embed.h" )
endif()

# The Python installed on the system; 3.9 is the oldest version pybind11 supports.
# Another installed Python can be chosen with Python_ROOT_DIR or Python_EXECUTABLE.
find_package( Python 3.9 REQUIRED COMPONENTS Interpreter Development.Embed )

# The installation directory (sys.base_prefix). The runner uses it as the Python home, so the interpreter
# uses exactly the standard library of this installation.
execute_process(
    COMMAND "${Python_EXECUTABLE}" -c "import sys; print( sys.base_prefix, end='' )"
    OUTPUT_VARIABLE CSB_PYTHON_HOME
    RESULT_VARIABLE rc )
if( NOT rc EQUAL 0 OR CSB_PYTHON_HOME STREQUAL "" )
    message( FATAL_ERROR "Could not determine sys.base_prefix of ${Python_EXECUTABLE}" )
endif()
file( TO_CMAKE_PATH "${CSB_PYTHON_HOME}" CSB_PYTHON_HOME )

csb_git_describe( "${CSB_PYBIND11_DIR}" CSB_PYBIND11_GIT_VERSION )
message( STATUS "Python runner: Python ${Python_VERSION} (${CSB_PYTHON_HOME}), pybind11 ${CSB_PYBIND11_DIR} (${CSB_PYBIND11_GIT_VERSION})" )

add_executable( csb_python main.cpp )
target_include_directories( csb_python SYSTEM PRIVATE "${CSB_PYBIND11_DIR}/include" )
target_compile_definitions( csb_python PRIVATE
    CSB_PYTHON_HOME="${CSB_PYTHON_HOME}"
    CSB_PYBIND11_GIT_VERSION="${CSB_PYBIND11_GIT_VERSION}" )
target_link_libraries( csb_python PRIVATE csb_benchmarks Python::Python )
csb_apply_compiler_options( csb_python )

if( WIN32 )
    # The Python DLL is copied next to the runner, so exactly this Python is loaded, not the first one in PATH.
    add_custom_command( TARGET csb_python POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different $<TARGET_RUNTIME_DLLS:csb_python> $<TARGET_FILE_DIR:csb_python>
        COMMAND_EXPAND_LISTS )
endif()
