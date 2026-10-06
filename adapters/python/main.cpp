/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */

// The Python runner: CPython embedded with pybind11. It uses the Python installation found by CMake (the system's
// Python, see PLAN.md); its exact version and build are part of the version string.
// CPython has one way of running code, so there is one variant. A free-threaded build (without the GIL) is
// reported as the variant "free-threaded", which is not part of the main comparison.

#include "registry.hpp"

#include <csb/runner.hpp>

#include <pybind11/embed.h>

#include <chrono>
#include <optional>
#include <stdexcept>


namespace py = pybind11;

namespace {

#if defined( Py_GIL_DISABLED )
constexpr char const variant_name[] = "free-threaded";
constexpr bool       is_headline    = false;
#else
constexpr char const variant_name[] = "";
constexpr bool       is_headline    = true;
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
    std::optional<py::scoped_interpreter>  mInterpreter;
    py::object                             mGlobals;  // the namespace of the script, like the one of a module
    py::object                             mBench;    // the script function bench, looked up once after loading

public:
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

            if( not globals.contains( "bench" ) || not PyCallable_Check( globals["bench"].ptr() ) ) {
                throw std::runtime_error( "the script does not define the function bench( n )" );
            }
            mBench   = globals["bench"];
            mGlobals = std::move( globals );
            return phases;
        } );
    }

    std::int64_t call_bench( std::int64_t const n ) override
    {
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
        mGlobals = py::object();
        mInterpreter.reset();
    }
};

} // namespace


int main( int argc, char *argv[] )
{
    csb::LanguageEntry const entry{
        "python",
        { variant_name },
        variant_name,
        []( std::string const & ) -> std::unique_ptr<csb::Language> { return std::make_unique<PythonLanguage>(); },
        is_headline ? std::optional<std::string>( variant_name ) : std::nullopt
    };
    return csb::run_main( argc, argv, entry, csb::all_benchmarks() );
}
