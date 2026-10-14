# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

set( CSB_AS_SOURCE_DIR "${CSB_ANGELSCRIPT_DIR}/angelscript/source" )
set( CSB_AS_ADDON_DIR  "${CSB_ANGELSCRIPT_DIR}/add_on" )

if( NOT EXISTS "${CSB_ANGELSCRIPT_DIR}/angelscript/include/angelscript.h" OR NOT EXISTS "${CSB_AS_ADDON_DIR}/scriptarray/scriptarray.h" )
    message( FATAL_ERROR "CSB_WITH_ANGELSCRIPT is ON, but CSB_ANGELSCRIPT_DIR ('${CSB_ANGELSCRIPT_DIR}') does not contain angelscript/include/angelscript.h and add_on" )
endif()

file( STRINGS "${CSB_ANGELSCRIPT_DIR}/angelscript/include/angelscript.h" CSB_AS_VERSION_LINE REGEX "#define ANGELSCRIPT_VERSION_STRING" )
string( REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+[^\"]*" CSB_AS_VERSION "${CSB_AS_VERSION_LINE}" )
message( STATUS "AngelScript runners: ${CSB_ANGELSCRIPT_DIR} (${CSB_AS_VERSION})" )

# The library sources for x64 on Windows (MSVC) and Linux (GCC), like the CMake project of the SDK
# (angelscript/projects/cmake), plus the add-ons for strings, arrays, dictionaries and math.
set( CSB_AS_SOURCES
    as_atomic.cpp as_builder.cpp as_bytecode.cpp as_callfunc.cpp as_callfunc_x64_gcc.cpp as_callfunc_x64_msvc.cpp
    as_compiler.cpp as_configgroup.cpp as_context.cpp as_datatype.cpp as_gc.cpp as_generic.cpp as_globalproperty.cpp
    as_memory.cpp as_module.cpp as_objecttype.cpp as_outputbuffer.cpp as_parser.cpp as_restore.cpp as_scriptcode.cpp
    as_scriptengine.cpp as_scriptfunction.cpp as_scriptnode.cpp as_scriptobject.cpp as_string.cpp as_string_util.cpp
    as_thread.cpp as_tokenizer.cpp as_typeinfo.cpp as_variablescope.cpp )
list( TRANSFORM CSB_AS_SOURCES PREPEND "${CSB_AS_SOURCE_DIR}/" )
if( MSVC )
    list( APPEND CSB_AS_SOURCES "${CSB_AS_SOURCE_DIR}/as_callfunc_x64_msvc_asm.asm" )
endif()
list( APPEND CSB_AS_SOURCES
    "${CSB_AS_ADDON_DIR}/scriptstdstring/scriptstdstring.cpp"
    "${CSB_AS_ADDON_DIR}/scriptstdstring/scriptstdstring_utils.cpp"
    "${CSB_AS_ADDON_DIR}/scriptarray/scriptarray.cpp"
    "${CSB_AS_ADDON_DIR}/scriptdictionary/scriptdictionary.cpp"
    "${CSB_AS_ADDON_DIR}/scriptmath/scriptmath.cpp" )

find_package( Threads REQUIRED )

# The library and the runner are built twice: in AngelScript's default (thread-safe) configuration
# and with AS_NO_THREADS.
function( csb_add_angelscript_runner target no_threads )
    add_library( ${target}_lib STATIC ${CSB_AS_SOURCES} )
    target_include_directories( ${target}_lib SYSTEM PUBLIC "${CSB_ANGELSCRIPT_DIR}/angelscript/include" "${CSB_AS_ADDON_DIR}" )
    if( no_threads )
        target_compile_definitions( ${target}_lib PUBLIC AS_NO_THREADS )
    endif()
    # The math add-on registers its functions for float by default (e.g. 'float sqrt(float)'). AS_USE_FLOAT=0 registers
    # the double versions instead (documented in the add-on's manual page), so AngelScript computes in double precision
    # like the other languages.
    target_compile_definitions( ${target}_lib PRIVATE AS_USE_FLOAT=0 )
    target_link_libraries( ${target}_lib PUBLIC Threads::Threads )
    csb_apply_compiler_options( ${target}_lib )
    if( MSVC )
        # C4828: scriptstdstring.cpp has two comments which are not valid UTF-8.
        target_compile_options( ${target}_lib PRIVATE $<$<COMPILE_LANGUAGE:CXX>:/wd4828> )
    else()
        # The VM reads and writes values of different types through casted pointers. The makefile of the SDK
        # (angelscript/projects/gnuc) therefore compiles with -fno-strict-aliasing; without it, GCC warns that
        # the optimizer may break this code. -w: no warnings from this third-party code.
        target_compile_options( ${target}_lib PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fno-strict-aliasing -w> )
    endif()

    add_executable( ${target} main.cpp )
    target_link_libraries( ${target} PRIVATE csb_benchmarks ${target}_lib )
    csb_apply_compiler_options( ${target} )
endfunction()

csb_add_angelscript_runner( csb_angelscript OFF )
csb_add_angelscript_runner( csb_angelscript_nothreads ON )
