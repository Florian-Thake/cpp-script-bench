/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */

// The Lua runner: Lua (compiled as C, like the official distribution) used through its C API, the canonical way
// to call Lua functions from C and C++. LuaBridge3 will be used for binding C++ functions and classes (interop).
// Lua has one way of running code, so there is one variant.

#include "registry.hpp"

#include <csb/runner.hpp>

#include <lua.hpp>

#include <chrono>
#include <stdexcept>


namespace {

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


class LuaLanguage final : public csb::Language
{
    lua_State  *mL = nullptr;

public:
    std::string name() const override { return "Lua"; }

    std::string version() const override
    {
        return LUA_VERSION_MAJOR "." LUA_VERSION_MINOR "." LUA_VERSION_RELEASE;
    }

    std::string script_extension() const override { return ".lua"; }

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
    csb::LanguageEntry const entry{
        "lua",
        { "" },
        "",
        []( std::string const & ) -> std::unique_ptr<csb::Language> { return std::make_unique<LuaLanguage>(); },
        ""
    };
    return csb::run_main( argc, argv, entry, csb::all_benchmarks() );
}
