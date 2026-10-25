/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */

// The Lua runner: Lua (compiled as C, like the official distribution) used through its C API, the canonical way
// to call Lua functions from C and C++. Lua has one way of running code; the variants differ in how the interop
// benchmarks bind C++:
//   (no name) : with LuaBridge3, the C++ binding library (the headline).
//   c-api     : with the plain Lua C API. Runs only the interop benchmarks, the others are the same in both.

#include "array_exchange/host.hpp"
#include "buffer/host.hpp"
#include "cpp_objects/host.hpp"
#include "csb_luabridge3_git_version.h"
#include "host_to_script/host.hpp"
#include "registry.hpp"
#include "script_to_host/host.hpp"
#include "string_exchange/host.hpp"

#include <csb/runner.hpp>

#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>
#include <LuaBridge/Vector.h>

#include <chrono>
#include <initializer_list>
#include <new>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>


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


// cpp_objects with the plain C API: a full userdata with a metatable, whose __index and __newindex map the field
// names to the members (like the array example of "Programming in Lua").
namespace c_api_cpp_objects {

using csb::host::cpp_objects::Vec3i;

constexpr char metatable_name[] = "Vec3i";

constexpr char const *field_names[] = { "x", "y", "z", nullptr };
constexpr int Vec3i::*fields[]      = { &Vec3i::x, &Vec3i::y, &Vec3i::z };

/// Vec3i(): creates a new Vec3i.
int create( lua_State *const L )
{
    new( lua_newuserdatauv( L, sizeof( Vec3i ), 0 ) ) Vec3i();
    luaL_setmetatable( L, metatable_name );
    return 1;
}

/// v.x: reads a field.
int index( lua_State *const L )
{
    auto const &v = *static_cast<Vec3i *>(luaL_checkudata( L, 1, metatable_name ));
    lua_pushinteger( L, v.*fields[luaL_checkoption( L, 2, nullptr, field_names )] );
    return 1;
}

/// v.x = value: writes a field.
int new_index( lua_State *const L )
{
    auto &v = *static_cast<Vec3i *>(luaL_checkudata( L, 1, metatable_name ));
    v.*fields[luaL_checkoption( L, 2, nullptr, field_names )] = static_cast<int>(luaL_checkinteger( L, 3 ));
    return 0;
}

constexpr luaL_Reg functions[] = {
    { "Vec3i", &create },
    { nullptr, nullptr },
};

} // namespace c_api_cpp_objects

/// Binds the C++ type of cpp_objects as 'Vec3i' in the global table 'host'.
void bind_cpp_objects( lua_State *const L, bool const c_api )
{
    if( c_api ) {
        luaL_newmetatable( L, c_api_cpp_objects::metatable_name );
        lua_pushcfunction( L, &c_api_cpp_objects::index );
        lua_setfield( L, -2, "__index" );
        lua_pushcfunction( L, &c_api_cpp_objects::new_index );
        lua_setfield( L, -2, "__newindex" );
        lua_pop( L, 1 );
        luaL_newlib( L, c_api_cpp_objects::functions );
        lua_setglobal( L, "host" );
    } else {
        using csb::host::cpp_objects::Vec3i;
        luabridge::getGlobalNamespace( L )
            .beginNamespace( "host" )
                .beginClass<Vec3i>( "Vec3i" )
                    .addConstructor<void (*)()>()
                    .addProperty( "x", &Vec3i::x, &Vec3i::x )
                    .addProperty( "y", &Vec3i::y, &Vec3i::y )
                    .addProperty( "z", &Vec3i::z, &Vec3i::z )
                .endClass()
            .endNamespace();
    }
}


// buffer: the image of the host as a userdata with the method set( index, value ). The index runs from 1, like in a
// Lua array. The usual object form of the C API: the metatable's __index is the table of the methods.
namespace c_api_buffer {

using csb::host::buffer::Image;

constexpr char metatable_name[] = "Image";

/// image:set( index, value ): writes a pixel. The userdata holds a pointer to the image of the host.
int set( lua_State *const L )
{
    auto &image = **static_cast<Image **>(luaL_checkudata( L, 1, metatable_name ));
    lua_Integer const index = luaL_checkinteger( L, 2 );
    luaL_argcheck( L, 1 <= index && index <= static_cast<lua_Integer>(image.pixels.size()), 2, "index out of range" );
    image.pixels[static_cast<std::size_t>(index - 1)] = static_cast<std::uint32_t>(luaL_checkinteger( L, 3 ));
    return 0;
}

constexpr luaL_Reg methods[] = {
    { "set", &set },
    { nullptr, nullptr },
};

} // namespace c_api_buffer

/// image:set( index, value ) with LuaBridge3: writes a pixel; the index runs from 1, like in a Lua array.
void image_set( csb::host::buffer::Image *const image, lua_Integer const index, std::uint32_t const value, lua_State *const L )
{
    if( index < 1 || index > static_cast<lua_Integer>(image->pixels.size()) ) {
        luaL_error( L, "image:set(): index out of range" );
    }
    image->pixels[static_cast<std::size_t>(index - 1)] = value;
}

/// Binds \param image, the image of the host for buffer, as 'image' in the global table 'host'.
void bind_buffer( lua_State *const L, bool const c_api, csb::host::buffer::Image &image )
{
    if( c_api ) {
        luaL_newmetatable( L, c_api_buffer::metatable_name );
        luaL_newlib( L, c_api_buffer::methods );
        lua_setfield( L, -2, "__index" );
        lua_pop( L, 1 );
        lua_newtable( L );
        *static_cast<csb::host::buffer::Image **>(lua_newuserdatauv( L, sizeof( csb::host::buffer::Image * ), 0 )) = &image;
        luaL_setmetatable( L, c_api_buffer::metatable_name );
        lua_setfield( L, -2, "image" );
        lua_setglobal( L, "host" );
    } else {
        luabridge::getGlobalNamespace( L )
            .beginNamespace( "host" )
                .beginClass<csb::host::buffer::Image>( "Image" )
                    .addFunction( "set", &image_set )
                .endClass()
                .addVariable( "image", &image )  // a pointer: the object of the host, not a copy
            .endNamespace();
    }
}


/// Throws if \param name is not a global function of \param L.
void check_global_function( lua_State *const L, char const *const name )
{
    int const type = lua_getglobal( L, name );
    lua_pop( L, 1 );
    if( type != LUA_TFUNCTION ) {
        throw std::runtime_error( "the script does not define the global function " + std::string( name ) );
    }
}

/// \return a reference in the registry to the global function \param name: the C API's handle for repeated calls.
int ref_global_function( lua_State *const L, char const *const name )
{
    check_global_function( L, name );
    lua_getglobal( L, name );
    return luaL_ref( L, LUA_REGISTRYINDEX );
}

/// Calls the function with the registry reference \param ref with the integer arguments \param args through the
/// C API. \return its integer result.
lua_Integer call_ref( lua_State *const L, int const ref, std::initializer_list<lua_Integer> const args )
{
    lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
    for( auto const arg : args ) {
        lua_pushinteger( L, arg );
    }
    if( lua_pcall( L, static_cast<int>(args.size()), 1, 0 ) != LUA_OK ) {
        throw std::runtime_error( "Lua error: " + pop_error( L ) );
    }
    int is_number = 0;
    lua_Integer const result = lua_tointegerx( L, -1, &is_number );
    lua_pop( L, 1 );
    if( not is_number ) {
        throw std::runtime_error( "a script function must return an integer" );
    }
    return result;
}

/// Calls the function with the registry reference \param ref with the string \param s through the C API.
/// \return its string result.
std::string call_ref( lua_State *const L, int const ref, std::string const &s )
{
    lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
    lua_pushlstring( L, s.data(), s.size() );
    if( lua_pcall( L, 1, 1, 0 ) != LUA_OK ) {
        throw std::runtime_error( "Lua error: " + pop_error( L ) );
    }
    std::size_t length = 0;
    char const *const chars = lua_tolstring( L, -1, &length );
    if( chars == nullptr ) {
        lua_pop( L, 1 );
        throw std::runtime_error( "a script function must return a string" );
    }
    std::string result( chars, length );
    lua_pop( L, 1 );
    return result;
}

/// Calls the function with the registry reference \param ref with a sequence (a table) of the elements of \param a
/// through the C API. \return the elements of the sequence it returns.
std::vector<int> call_ref( lua_State *const L, int const ref, std::vector<int> const &a )
{
    lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
    lua_createtable( L, static_cast<int>(a.size()), 0 );
    for( std::size_t i = 0; i < a.size(); ++i ) {
        lua_pushinteger( L, a[i] );
        lua_rawseti( L, -2, static_cast<lua_Integer>(i + 1) );
    }
    if( lua_pcall( L, 1, 1, 0 ) != LUA_OK ) {
        throw std::runtime_error( "Lua error: " + pop_error( L ) );
    }
    if( not lua_istable( L, -1 ) ) {
        lua_pop( L, 1 );
        throw std::runtime_error( "a script function must return a table" );
    }
    auto const size = static_cast<lua_Integer>(lua_rawlen( L, -1 ));
    std::vector<int> b;
    b.reserve( static_cast<std::size_t>(size) );
    for( lua_Integer i = 1; i <= size; ++i ) {
        lua_rawgeti( L, -1, i );
        b.push_back( static_cast<int>(lua_tointeger( L, -1 )) );
        lua_pop( L, 1 );
    }
    lua_pop( L, 1 );
    return b;
}

/// \return the result of a call with LuaBridge3, or throws its error.
template< typename T >
T result_of( luabridge::TypeResult<T> const &result )
{
    if( not result ) {
        throw std::runtime_error( "Lua error: " + result.message() );
    }
    return *result;
}


class LuaLanguage final : public csb::Language
{
    Variant           mVariant;
    lua_State        *mL = nullptr;
    std::string_view  mBenchmark;  // the name of the loaded benchmark
    // The script functions of host_to_script, looked up once: LuaRefs, or the registry references of the C API.
    std::optional<luabridge::LuaRef>  mF0;
    std::optional<luabridge::LuaRef>  mF1;
    std::optional<luabridge::LuaRef>  mF3;
    int                               mRef0 = LUA_NOREF;
    int                               mRef1 = LUA_NOREF;
    int                               mRef3 = LUA_NOREF;
    // The script function f of string_exchange and array_exchange, the same way.
    std::optional<luabridge::LuaRef>  mF;
    int                               mRefF = LUA_NOREF;
    csb::host::buffer::Image          mImage;  // buffer: the image of the host, 'host.image' in the script

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
        } else if( info.name == "cpp_objects" ) {
            bind_cpp_objects( mL, mVariant.c_api );
        } else if( info.name == "buffer" ) {
            bind_buffer( mL, mVariant.c_api, mImage );
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

        mBenchmark = info.name;
        if( mBenchmark == "host_to_script" ) {
            if( mVariant.c_api ) {
                mRef0 = ref_global_function( mL, "f0" );
                mRef1 = ref_global_function( mL, "f1" );
                mRef3 = ref_global_function( mL, "f3" );
            } else {
                for( char const *const name : { "f0", "f1", "f3" } ) {
                    check_global_function( mL, name );
                }
                mF0 = luabridge::getGlobal( mL, "f0" );
                mF1 = luabridge::getGlobal( mL, "f1" );
                mF3 = luabridge::getGlobal( mL, "f3" );
            }
        } else if( mBenchmark == "string_exchange" || mBenchmark == "array_exchange" ) {
            if( mVariant.c_api ) {
                mRefF = ref_global_function( mL, "f" );
            } else {
                check_global_function( mL, "f" );
                mF = luabridge::getGlobal( mL, "f" );
            }
        } else {
            check_global_function( mL, "bench" );
        }
        return phases;
    }

    std::int64_t call_bench( std::int64_t const n ) override
    {
        if( mBenchmark == "host_to_script" ) {
            return call_host_to_script( n );
        }
        if( mBenchmark == "array_exchange" ) {
            // LuaBridge3: call<std::vector<int>>() through the LuaRef, which converts both ways (LuaBridge/Vector.h).
            // C API: lua_createtable() and lua_rawseti(), lua_pcall(), lua_rawlen() and lua_rawgeti().
            if( mVariant.c_api ) {
                return csb::host::array_exchange::run( n, [&]( std::vector<int> const &a ) { return call_ref( mL, mRefF, a ); } );
            }
            return csb::host::array_exchange::run( n, [&]( std::vector<int> const &a ) { return result_of( mF->call<std::vector<int>>( a ) ); } );
        }
        if( mBenchmark == "string_exchange" ) {
            // LuaBridge3: call<std::string>() through the LuaRef. C API: lua_pushlstring(), lua_pcall(), lua_tolstring().
            if( mVariant.c_api ) {
                return csb::host::string_exchange::run( n, [&]( std::string const &s ) { return call_ref( mL, mRefF, s ); } );
            }
            return csb::host::string_exchange::run( n, [&]( std::string const &s ) { return result_of( mF->call<std::string>( s ) ); } );
        }
        if( mBenchmark == "buffer" ) {
            // The host resizes and clears its image, the script fills it, the host computes the checksum over it.
            mImage.reset( n );
            return csb::host::buffer::result( n, call_script_bench( n ), mImage.bytes() );
        }
        return call_script_bench( n );
    }

    void collect_garbage() override
    {
        lua_gc( mL, LUA_GCCOLLECT );
    }

    void destroy_engine() override
    {
        // The LuaRefs must be released while the state still exists. The registry references end with the state.
        mF0.reset();
        mF1.reset();
        mF3.reset();
        mF.reset();
        mRef0 = mRef1 = mRef3 = mRefF = LUA_NOREF;
        if( mL != nullptr ) {
            lua_close( mL );
            mL = nullptr;
        }
    }

private:
    /// Calls bench( n ) of the script. \return its result.
    std::int64_t call_script_bench( std::int64_t const n )
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

    /// The host's loop of host_to_script. LuaBridge3: calls through LuaRefs, looked up once.
    /// C API: lua_pcall() of the functions from their registry references (luaL_ref), looked up once.
    std::int64_t call_host_to_script( std::int64_t const n )
    {
        namespace host = csb::host::host_to_script;
        if( mVariant.c_api ) {
            return host::run(
                n,
                [&]() { return call_ref( mL, mRef0, {} ); },
                [&]( int const a ) { return call_ref( mL, mRef1, { a } ); },
                [&]( int const a, int const b, int const c ) { return call_ref( mL, mRef3, { a, b, c } ); } );
        }
        return host::run(
            n,
            [&]() { return result_of( mF0->call<int>() ); },
            [&]( int const a ) { return result_of( mF1->call<int>( a ) ); },
            [&]( int const a, int const b, int const c ) { return result_of( mF3->call<int>( a, b, c ) ); } );
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
