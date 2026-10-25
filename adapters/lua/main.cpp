/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */

// The Lua runner: Lua (compiled as C, like the official distribution) used through its C API, the canonical way
// to call Lua functions from C and C++. Lua has one way of running code; the variants differ in how the interop
// benchmarks bind C++:
//   (no name) : with LuaBridge3, the C++ binding library (the headline).
//   c-api     : with the plain Lua C API. Runs only the interop benchmarks, the others are the same in both.

#include "csb_luabridge3_git_version.h"
#include "registry.hpp"
#include "script_to_host/host.hpp"

#include <csb/runner.hpp>

#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>

#include <chrono>
#include <stdexcept>


namespace {

struct Variant
{
    std::string_view  name;
    bool              c_api;  ///< true: the interop benchmarks bind C++ with the plain C API instead of LuaBridge3
};

constexpr Variant variants[] = {
    { "",      false },
    { "c-api", true },
};


using Clock = std::chrono::steady_clock;

double seconds_since( Clock::time_point const start )
{
    return std::chrono::duration<double>( Clock::now() - start ).count();
}

/// Pops the error object of a failed Lua API call from the stack and returns it as a message.
std::string pop_error( lua_State *const L )
{
    std::string msg = lua_isstring( L, -1 ) ? lua_tostring( L, -1 ) : "(the error object is not a string)";
    lua_pop( L, 1 );
    return msg;
}


// script_to_host with the plain C API: lua_CFunctions, which take their arguments from the stack.
namespace c_api_script_to_host {

namespace host = csb::host::script_to_host;

int f0( lua_State *const L )
{
    lua_pushinteger( L, host::f0() );
    return 1;
}

int f1( lua_State *const L )
{
    lua_pushinteger( L, host::f1( static_cast<int>(luaL_checkinteger( L, 1 )) ) );
    return 1;
}

int f3( lua_State *const L )
{
    lua_pushinteger( L, host::f3( static_cast<int>(luaL_checkinteger( L, 1 )), static_cast<int>(luaL_checkinteger( L, 2 )),
                                  static_cast<int>(luaL_checkinteger( L, 3 )) ) );
    return 1;
}

constexpr luaL_Reg functions[] = {
    { "f0", &f0 },
    { "f1", &f1 },
    { "f3", &f3 },
    { nullptr, nullptr },
};

} // namespace c_api_script_to_host

/// Registers the C++ functions of script_to_host in the global table 'host'.
void bind_script_to_host( lua_State *const L, bool const c_api )
{
    if( c_api ) {
        luaL_newlib( L, c_api_script_to_host::functions );
        lua_setglobal( L, "host" );
    } else {
        namespace host = csb::host::script_to_host;
        luabridge::getGlobalNamespace( L )
            .beginNamespace( "host" )
                .addFunction( "f0", &host::f0 )
                .addFunction( "f1", &host::f1 )
                .addFunction( "f3", &host::f3 )
            .endNamespace();
    }
}


class LuaLanguage final : public csb::Language
{
    Variant     mVariant;
    lua_State  *mL = nullptr;

public:
    explicit LuaLanguage( Variant const &rVariant )
        : mVariant( rVariant )
    {
    }

    std::string name() const override { return "Lua"; }

    std::string version() const override
    {
        return LUA_VERSION_MAJOR "." LUA_VERSION_MINOR "." LUA_VERSION_RELEASE "; LuaBridge3 (" CSB_LUABRIDGE3_GIT_VERSION ")";
    }

    std::string script_extension() const override { return ".lua"; }

    bool supports( csb::BenchmarkInfo const &info ) const override
    {
        return not mVariant.c_api || info.group == "interop";
    }

    void create_engine() override
    {
        mL = luaL_newstate();
        if( mL == nullptr ) {
            throw std::runtime_error( "luaL_newstate() failed" );
        }
        // The full standard library, like the other languages get theirs, but without loading modules ('package'
        // with 'require') and script files ('dofile', 'loadfile'), so there is no disk IO. This matches the
        // options No_Load_Modules and No_External_Scripts of the ChaiScript runner.
        luaL_openselectedlibs( mL, ~LUA_LOADLIBK, 0 );
        lua_pushnil( mL );
        lua_setglobal( mL, "dofile" );
        lua_pushnil( mL );
        lua_setglobal( mL, "loadfile" );
    }

    void bind( csb::BenchmarkInfo const &info ) override
    {
        if( info.name == "script_to_host" ) {
            bind_script_to_host( mL, mVariant.c_api );
        }
    }

    std::vector<csb::Phase> load( csb::BenchmarkInfo const &info, std::string const &source ) override
    {
        std::vector<csb::Phase> phases;
        std::string const chunk_name = "=" + std::string( info.name ) + script_extension();

        // Lua parses and compiles in one pass. Only text is accepted, no precompiled chunks.
        auto start = Clock::now();
        if( luaL_loadbufferx( mL, source.data(), source.size(), chunk_name.c_str(), "t" ) != LUA_OK ) {
            throw std::runtime_error( "Lua syntax error: " + pop_error( mL ) );
        }
        phases.push_back( { "parse_compile", seconds_since( start ) } );

        start = Clock::now();
        if( lua_pcall( mL, 0, 0, 0 ) != LUA_OK ) {
            throw std::runtime_error( "Lua error: " + pop_error( mL ) );
        }
        phases.push_back( { "top_level", seconds_since( start ) } );

        if( lua_getglobal( mL, "bench" ) != LUA_TFUNCTION ) {
            lua_pop( mL, 1 );
            throw std::runtime_error( "the script does not define the global function bench( n )" );
        }
        lua_pop( mL, 1 );
        return phases;
    }

    std::int64_t call_bench( std::int64_t const n ) override
    {
        lua_getglobal( mL, "bench" );
        lua_pushinteger( mL, static_cast<lua_Integer>(n) );
        if( lua_pcall( mL, 1, 1, 0 ) != LUA_OK ) {
            throw std::runtime_error( "Lua error: " + pop_error( mL ) );
        }
        // Only an integer is accepted. lua_tointegerx() would also convert e.g. a float with an integral value,
        // or a numeric string.
        if( not lua_isinteger( mL, -1 ) ) {
            std::string const type = lua_type( mL, -1 ) == LUA_TNUMBER ? "float" : luaL_typename( mL, -1 );
            lua_pop( mL, 1 );
            throw std::runtime_error( "bench() must return an integer, but returned: " + type );
        }
        lua_Integer const value = lua_tointeger( mL, -1 );
        lua_pop( mL, 1 );
        return static_cast<std::int64_t>(value);
    }

    void collect_garbage() override
    {
        lua_gc( mL, LUA_GCCOLLECT );
    }

    void destroy_engine() override
    {
        if( mL != nullptr ) {
            lua_close( mL );
            mL = nullptr;
        }
    }
};

} // namespace


int main( int argc, char *argv[] )
{
    csb::LanguageEntry entry;
    entry.id               = "lua";
    entry.default_variant  = "";
    entry.headline_variant = "";
    for( auto const &variant : variants ) {
        entry.variants.emplace_back( variant.name );
    }
    entry.make = []( std::string const &name ) -> std::unique_ptr<csb::Language> {
        for( auto const &variant : variants ) {
            if( variant.name == name ) {
                return std::make_unique<LuaLanguage>( variant );
            }
        }
        throw std::invalid_argument( "unknown Lua variant: " + name );
    };

    return csb::run_main( argc, argv, entry, csb::all_benchmarks() );
}
