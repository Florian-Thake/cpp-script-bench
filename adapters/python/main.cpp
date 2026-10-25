/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */

// The Python runner: CPython embedded with pybind11. It uses the Python installation found by CMake (the system's
// Python, see PLAN.md); its exact version and build are part of the version string.
// CPython has one way of running code; the variants differ in how the interop benchmarks bind C++:
//   (no name) : with pybind11, the C++ binding library (the headline).
//   c-api     : with the plain Python C API. Runs only the interop benchmarks, the others are the same in both.
// A free-threaded build (without the GIL) reports them as "free-threaded" and "free-threaded-c-api", which are not
// part of the main comparison.

#include "csb_pybind11_git_version.h"
#include "host_to_script/host.hpp"
#include "registry.hpp"
#include "script_to_host/host.hpp"

#include <csb/runner.hpp>

#include <pybind11/embed.h>

#include <chrono>
#include <climits>
#include <initializer_list>
#include <optional>
#include <stdexcept>


namespace py = pybind11;

// The module 'host', which the scripts of the interop benchmarks import. With pybind11 it is an embedded module;
// bind() adds what the benchmark needs, so every benchmark binds only its own functions.
PYBIND11_EMBEDDED_MODULE( host, m, py::mod_gil_not_used() )
{
    m.doc() = "C++ functions, types and data of the host for the interop benchmarks";
}

namespace {

struct Variant
{
    std::string_view  name;
    bool              c_api;  ///< true: the interop benchmarks bind C++ with the plain C API instead of pybind11
};

#if defined( Py_GIL_DISABLED )
constexpr Variant variants[] = {
    { "free-threaded",       false },
    { "free-threaded-c-api", true },
};
std::optional<std::string> const headline_variant = std::nullopt;
#else
constexpr Variant variants[] = {
    { "",      false },
    { "c-api", true },
};
std::optional<std::string> const headline_variant = "";
#endif


using Clock = std::chrono::steady_clock;

double seconds_since( Clock::time_point const start )
{
    return std::chrono::duration<double>( Clock::now() - start ).count();
}

/// Runs \param func and converts Python errors into std::runtime_error with a helpful message.
template< typename F >
auto translate_errors( F &&func ) -> decltype(func())
{
    try {
        return func();
    } catch( py::error_already_set const &ex ) {
        throw std::runtime_error( std::string( "Python error: " ) + ex.what() );
    }
}

/// Converts \param obj to an int for a C API function. \return false with a Python exception set on errors.
bool to_int( PyObject *const obj, int &rValue )
{
    long const value = PyLong_AsLong( obj );
    if( value == -1 && PyErr_Occurred() ) {
        return false;
    }
    if( value < INT_MIN || value > INT_MAX ) {
        PyErr_SetString( PyExc_OverflowError, "the value does not fit into an int" );
        return false;
    }
    rValue = static_cast<int>(value);
    return true;
}

/// Creates the module 'host' with \param methods for the C API variant and puts it into sys.modules, where the
/// import of the script finds it.
void add_c_api_host_module( PyMethodDef *const methods )
{
    auto const module = py::reinterpret_steal<py::object>( PyModule_New( "host" ) );
    if( not module ) {
        throw py::error_already_set();
    }
#if defined( Py_GIL_DISABLED )
    PyUnstable_Module_SetGIL( module.ptr(), Py_MOD_GIL_NOT_USED );
#endif
    if( PyModule_AddFunctions( module.ptr(), methods ) < 0 || PyDict_SetItemString( PyImport_GetModuleDict(), "host", module.ptr() ) < 0 ) {
        throw py::error_already_set();
    }
}


// script_to_host with the plain C API: the calling conventions with the least overhead for 0, 1 and 3 arguments.
namespace c_api_script_to_host {

namespace host = csb::host::script_to_host;

PyObject *f0( PyObject *, PyObject * )
{
    return PyLong_FromLong( host::f0() );
}

PyObject *f1( PyObject *, PyObject *const arg )
{
    int a = 0;
    if( not to_int( arg, a ) ) {
        return nullptr;
    }
    return PyLong_FromLong( host::f1( a ) );
}

PyObject *f3( PyObject *, PyObject *const *const args, Py_ssize_t const nargs )
{
    if( nargs != 3 ) {
        PyErr_SetString( PyExc_TypeError, "f3() takes exactly 3 arguments" );
        return nullptr;
    }
    int a = 0, b = 0, c = 0;
    if( not to_int( args[0], a ) || not to_int( args[1], b ) || not to_int( args[2], c ) ) {
        return nullptr;
    }
    return PyLong_FromLong( host::f3( a, b, c ) );
}

PyMethodDef methods[] = {
    { "f0", &f0, METH_NOARGS, nullptr },
    { "f1", &f1, METH_O, nullptr },
    { "f3", reinterpret_cast<PyCFunction>(reinterpret_cast<void (*)()>(&f3)), METH_FASTCALL, nullptr },
    { nullptr, nullptr, 0, nullptr },
};

} // namespace c_api_script_to_host

/// Adds the C++ functions of script_to_host to the module \param m with pybind11.
void bind_script_to_host( py::module_ &m )
{
    namespace host = csb::host::script_to_host;
    m.def( "f0", &host::f0 );
    m.def( "f1", &host::f1 );
    m.def( "f3", &host::f3 );
}


/// \return the function \param name of the script namespace \param globals.
py::object find_function( py::dict const &globals, char const *const name )
{
    if( not globals.contains( name ) || not PyCallable_Check( globals[name].ptr() ) ) {
        throw std::runtime_error( "the script does not define the function " + std::string( name ) );
    }
    return globals[name];
}

/// \return the int value of \param result, a new reference returned by a C API call, which this function releases.
/// \param result is nullptr if the call failed.
long take_long( PyObject *const result )
{
    if( result == nullptr ) {
        throw py::error_already_set();
    }
    long const value = PyLong_AsLong( result );
    Py_DECREF( result );
    if( value == -1 && PyErr_Occurred() ) {
        throw py::error_already_set();
    }
    return value;
}

/// Calls \param func with up to 3 int arguments \param args through the C API: the call functions with the least
/// overhead for 0, 1 and more arguments. \return its int result.
long call_c_api( PyObject *const func, std::initializer_list<long> const args )
{
    if( args.size() == 0 ) {
        return take_long( PyObject_CallNoArgs( func ) );
    }
    PyObject *objects[3] = {};
    size_t count = 0;
    for( long const arg : args ) {
        objects[count] = PyLong_FromLong( arg );
        if( objects[count] == nullptr ) {
            break;
        }
        ++count;
    }
    PyObject *result = nullptr;
    if( count == args.size() ) {
        result = count == 1 ? PyObject_CallOneArg( func, objects[0] ) : PyObject_Vectorcall( func, objects, count, nullptr );
    }
    for( size_t i = 0; i < count; ++i ) {
        Py_DECREF( objects[i] );
    }
    return take_long( result );
}


/// Throws if the experimental JIT of CPython is enabled. It must be off for the main comparison (PLAN.md, rule 8).
void check_jit_is_off()
{
    // sys._jit exists since Python 3.14.
    PyObject *const jit = PySys_GetObject( "_jit" );  // borrowed reference, no error set if missing
    if( jit != nullptr && py::handle( jit ).attr( "is_enabled" )().cast<bool>() ) {
        throw std::runtime_error( "the experimental JIT of CPython is enabled, but it must be off" );
    }
}


class PythonLanguage final : public csb::Language
{
    Variant                                mVariant;
    std::optional<py::scoped_interpreter>  mInterpreter;
    py::object                             mGlobals;  // the namespace of the script, like the one of a module
    py::object                             mBench;    // the script function bench, looked up once after loading
    std::string_view                       mBenchmark;  // the name of the loaded benchmark
    py::object                             mF0;       // the script functions of host_to_script, looked up once like bench
    py::object                             mF1;
    py::object                             mF3;

public:
    explicit PythonLanguage( Variant const &rVariant )
        : mVariant( rVariant )
    {
    }

    std::string name() const override { return "Python"; }

    std::string version() const override
    {
        // e.g. "3.14.5 (tags/v3.14.5:5607950, May 10 2026, 10:43:50) [MSC v.1944 64 bit (AMD64)]"
        std::string version = Py_GetVersion();
        for( auto &c : version ) {
            if( c == '\n' ) {
                c = ' ';
            }
        }
        return version + "; pybind11 " + std::to_string( PYBIND11_VERSION_MAJOR ) + "." + std::to_string( PYBIND11_VERSION_MINOR ) + "."
             + std::to_string( PYBIND11_VERSION_PATCH ) + " (" CSB_PYBIND11_GIT_VERSION ")";
    }

    std::string script_extension() const override { return ".py"; }

    bool supports( csb::BenchmarkInfo const &info ) const override
    {
        return not mVariant.c_api || info.group == "interop";
    }

    void create_engine() override
    {
        // The isolated configuration, which the Python documentation recommends for embedding: environment
        // variables (e.g. PYTHONPATH, PYTHON_JIT), the user site directory and the command line are ignored.
        PyConfig config;
        PyConfig_InitIsolatedConfig( &config );
        // No 'site' module (like 'python -S'): the site-packages directories and their .pth files are not
        // processed. That is disk IO which depends on the machine, and they are not part of the standard library.
        config.site_import = 0;
        // The installation found by CMake, so exactly its standard library is used.
        PyStatus const status = PyConfig_SetBytesString( &config, &config.home, CSB_PYTHON_HOME );
        if( PyStatus_Exception( status ) ) {
            PyConfig_Clear( &config );
            throw std::runtime_error( std::string( "setting the Python home failed: " ) + (status.err_msg ? status.err_msg : "") );
        }
        // false: the directory of the runner is not added to sys.path, nothing is imported from there.
        mInterpreter.emplace( &config, 0, nullptr, false );

        translate_errors( check_jit_is_off );
    }

    void bind( csb::BenchmarkInfo const &info ) override
    {
        translate_errors( [&]() {
            if( info.name == "script_to_host" ) {
                if( mVariant.c_api ) {
                    add_c_api_host_module( c_api_script_to_host::methods );
                } else {
                    auto m = py::module_::import( "host" );
                    bind_script_to_host( m );
                }
            }
        } );
    }

    std::vector<csb::Phase> load( csb::BenchmarkInfo const &info, std::string const &source ) override
    {
        return translate_errors( [&]() {
            std::vector<csb::Phase> phases;
            std::string const filename = std::string( info.name ) + script_extension();

            // A fresh namespace for the script, like the one of a module. Its functions look up globals in it.
            py::dict globals;
            globals["__name__"]     = std::string( info.name );
            globals["__builtins__"] = py::module_::import( "builtins" );

            // Python parses and compiles to bytecode in one step.
            auto start = Clock::now();
            auto const code = py::reinterpret_steal<py::object>( Py_CompileString( source.c_str(), filename.c_str(), Py_file_input ) );
            if( not code ) {
                throw py::error_already_set();
            }
            phases.push_back( { "parse_compile", seconds_since( start ) } );

            start = Clock::now();
            auto const result = py::reinterpret_steal<py::object>( PyEval_EvalCode( code.ptr(), globals.ptr(), globals.ptr() ) );
            if( not result ) {
                throw py::error_already_set();
            }
            phases.push_back( { "top_level", seconds_since( start ) } );

            mBenchmark = info.name;
            if( mBenchmark == "host_to_script" ) {
                mF0 = find_function( globals, "f0" );
                mF1 = find_function( globals, "f1" );
                mF3 = find_function( globals, "f3" );
            } else {
                mBench = find_function( globals, "bench" );
            }
            mGlobals = std::move( globals );
            return phases;
        } );
    }

    std::int64_t call_bench( std::int64_t const n ) override
    {
        if( mBenchmark == "host_to_script" ) {
            return translate_errors( [&]() { return call_host_to_script( n ); } );
        }
        return translate_errors( [&]() -> std::int64_t {
            py::object const res = mBench( static_cast<long long>(n) );
            // Only an int is accepted, not a subclass like bool. A conversion to an integer type would also accept
            // other types with __index__.
            if( not PyLong_CheckExact( res.ptr() ) ) {
                throw std::runtime_error( std::string( "bench() must return an int, but returned: " ) + Py_TYPE( res.ptr() )->tp_name );
            }
            int overflow = 0;
            long long const value = PyLong_AsLongLongAndOverflow( res.ptr(), &overflow );
            if( overflow != 0 ) {
                throw std::runtime_error( "the result of bench() does not fit into 64 bit" );
            }
            return static_cast<std::int64_t>(value);
        } );
    }

    void collect_garbage() override
    {
        PyGC_Collect();
    }

    void destroy_engine() override
    {
        // The references must be released while the interpreter still exists.
        mBench   = py::object();
        mF0      = py::object();
        mF1      = py::object();
        mF3      = py::object();
        mGlobals = py::object();
        mInterpreter.reset();
    }

private:
    /// The host's loop of host_to_script. pybind11: calls of the function objects with conversions by pybind11.
    /// C API: PyObject_CallNoArgs(), PyObject_CallOneArg() and PyObject_Vectorcall().
    std::int64_t call_host_to_script( std::int64_t const n )
    {
        namespace host = csb::host::host_to_script;
        if( mVariant.c_api ) {
            return host::run(
                n,
                [&]() { return call_c_api( mF0.ptr(), {} ); },
                [&]( int const a ) { return call_c_api( mF1.ptr(), { a } ); },
                [&]( int const a, int const b, int const c ) { return call_c_api( mF3.ptr(), { a, b, c } ); } );
        }
        return host::run(
            n,
            [&]() { return mF0().cast<int>(); },
            [&]( int const a ) { return mF1( a ).cast<int>(); },
            [&]( int const a, int const b, int const c ) { return mF3( a, b, c ).cast<int>(); } );
    }
};

} // namespace


int main( int argc, char *argv[] )
{
    csb::LanguageEntry entry;
    entry.id               = "python";
    entry.default_variant  = std::string( variants[0].name );
    entry.headline_variant = headline_variant;
    for( auto const &variant : variants ) {
        entry.variants.emplace_back( variant.name );
    }
    entry.make = []( std::string const &name ) -> std::unique_ptr<csb::Language> {
        for( auto const &variant : variants ) {
            if( variant.name == name ) {
                return std::make_unique<PythonLanguage>( variant );
            }
        }
        throw std::invalid_argument( "unknown Python variant: " + name );
    };

    return csb::run_main( argc, argv, entry, csb::all_benchmarks() );
}
