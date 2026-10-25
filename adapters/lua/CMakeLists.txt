# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

if( NOT EXISTS "${CSB_LUA_DIR}/src/lua.h" )
    message( FATAL_ERROR "CSB_WITH_LUA is ON, but CSB_LUA_DIR ('${CSB_LUA_DIR}') does not contain src/lua.h" )
endif()

# LuaBridge3 binds C++ functions and classes for the interop benchmarks (the headline variant). It is required, so the
# headline always exists.
if( NOT EXISTS "${CSB_LUABRIDGE3_DIR}/Source/LuaBridge/LuaBridge.h" )
    message( FATAL_ERROR "CSB_WITH_LUA is ON, but CSB_LUABRIDGE3_DIR ('${CSB_LUABRIDGE3_DIR}') does not contain Source/LuaBridge/LuaBridge.h" )
endif()

file( STRINGS "${CSB_LUA_DIR}/src/lua.h" CSB_LUA_VERSION_LINES REGEX "#define LUA_VERSION_(MAJOR|MINOR|RELEASE)_N[ \t]" )
string( REGEX REPLACE "[^0-9;]" "" CSB_LUA_VERSION "${CSB_LUA_VERSION_LINES}" )
string( REPLACE ";" "." CSB_LUA_VERSION "${CSB_LUA_VERSION}" )
csb_git_describe( "${CSB_LUABRIDGE3_DIR}" CSB_LUABRIDGE3_GIT_VERSION )
message( STATUS "Lua runner: ${CSB_LUA_DIR} (${CSB_LUA_VERSION}), LuaBridge3: ${CSB_LUABRIDGE3_DIR} (${CSB_LUABRIDGE3_GIT_VERSION})" )

# The Lua library, compiled as C like the official distribution does, with the same compiler
# options as everything else. All sources except the stand-alone programs lua.c and luac.c.
file( GLOB CSB_LUA_SOURCES "${CSB_LUA_DIR}/src/*.c" )
list( REMOVE_ITEM CSB_LUA_SOURCES "${CSB_LUA_DIR}/src/lua.c" "${CSB_LUA_DIR}/src/luac.c" )

add_library( csb_lua_lib STATIC ${CSB_LUA_SOURCES} )
target_include_directories( csb_lua_lib SYSTEM PUBLIC "${CSB_LUA_DIR}/src" )
if( CMAKE_SYSTEM_NAME STREQUAL "Linux" )
    # Like 'make linux' of the official Makefile.
    target_compile_definitions( csb_lua_lib PRIVATE LUA_USE_LINUX )
    target_link_libraries( csb_lua_lib PUBLIC m ${CMAKE_DL_LIBS} )
endif()
csb_apply_compiler_options( csb_lua_lib )

add_executable( csb_lua main.cpp )
target_include_directories( csb_lua SYSTEM PRIVATE "${CSB_LUABRIDGE3_DIR}/Source" )
target_link_libraries( csb_lua PRIVATE csb_benchmarks csb_lua_lib )
csb_target_git_version( csb_lua CSB_LUABRIDGE3_GIT_VERSION "${CSB_LUABRIDGE3_DIR}" )
csb_apply_compiler_options( csb_lua )
