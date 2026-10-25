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

#include "array_exchange/host.hpp"
#include "buffer/host.hpp"
#include "cpp_objects/host.hpp"
#include "csb_pybind11_git_version.h"
#include "host_to_script/host.hpp"
#include "registry.hpp"
#include "script_to_host/host.hpp"
#include "string_exchange/host.hpp"

#include <csb/runner.hpp>

#include <pybind11/embed.h>
#include <pybind11/stl.h>  // std::vector <-> list, for array_exchange
#if PY_VERSION_HEX < 0x030C0000
# include <structmember.h>  // T_INT, which Python 3.12 renamed to Py_T_INT
#endif

#include <chrono>
#include <climits>
#include <cstddef>
#include <initializer_list>
#include <new>
#include <optional>
#include <stdexcept>
#include <vector>


namespace py = pybind11;

#if PY_VERSION_HEX >= 0x030C0000
constexpr int member_type_int = Py_T_INT;
#else
constexpr int member_type_int = T_INT;
#endif

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

/// Creates the module 'host' with \param methods (may be nullptr) for the C API variant and puts it into sys.modules,
/// where the import of the script finds it. \return the module.
py::object add_c_api_host_module( PyMethodDef *const methods )
{
    auto module = py::reinterpret_steal<py::object>( PyModule_New( "host" ) );
    if( not module ) {
        throw py::error_already_set();
    }
#if defined( Py_GIL_DISABLED )
    PyUnstable_Module_SetGIL( module.ptr(), Py_MOD_GIL_NOT_USED );
#endif
    if( (methods != nullptr && PyModule_AddFunctions( module.ptr(), methods ) < 0)
        || PyDict_SetItemString( PyImport_GetModuleDict(), "host", module.ptr() ) < 0 ) {
        throw py::error_already_set();
    }
    return module;
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

// cpp_objects with the plain C API: a type whose fields are member descriptors at their offsets in the object.
namespace c_api_cpp_objects {

using csb::host::cpp_objects::Vec3i;

struct PyVec3i
{
    PyObject_HEAD
    Vec3i value;
};

PyObject *create( PyTypeObject *const type, PyObject *, PyObject * )
{
    PyObject *const self = type->tp_alloc( type, 0 );
    if( self != nullptr ) {
        new( &reinterpret_cast<PyVec3i *>(self)->value ) Vec3i();
    }
    return self;
}

PyMemberDef members[] = {
    { "x", member_type_int, offsetof( PyVec3i, value ) + offsetof( Vec3i, x ), 0, nullptr },
    { "y", member_type_int, offsetof( PyVec3i, value ) + offsetof( Vec3i, y ), 0, nullptr },
    { "z", member_type_int, offsetof( PyVec3i, value ) + offsetof( Vec3i, z ), 0, nullptr },
    { nullptr, 0, 0, 0, nullptr },
};

PyType_Slot slots[] = {
    { Py_tp_new, reinterpret_cast<void *>(&create) },
    { Py_tp_members, members },
    { 0, nullptr },
};

PyType_Spec spec = { "host.Vec3i", sizeof( PyVec3i ), 0, Py_TPFLAGS_DEFAULT, slots };

/// Creates the module 'host' with the type Vec3i.
void add_module()
{
    auto const module = add_c_api_host_module( nullptr );
    auto const type   = py::reinterpret_steal<py::object>( PyType_FromSpec( &spec ) );
    if( not type || PyObject_SetAttrString( module.ptr(), "Vec3i", type.ptr() ) < 0 ) {
        throw py::error_already_set();
    }
}

} // namespace c_api_cpp_objects

// buffer with the plain C API: the image of the host as an object with the buffer protocol.
namespace c_api_buffer {

using csb::host::buffer::Image;

struct PyImage
{
    PyObject_HEAD
    Image       *image;   // the image of the host
    Py_ssize_t   shape;   // the shape and the stride of the last view, which must live as long as the view
    Py_ssize_t   stride;
};

/// The buffer protocol: a writable 1-dimensional view of the pixels (format 'I', 4 bytes each) in the memory of the host.
int get_buffer( PyObject *const self, Py_buffer *const view, int const flags )
{
    auto &object   = *reinterpret_cast<PyImage *>(self);
    object.shape   = static_cast<Py_ssize_t>(object.image->pixels.size());
    object.stride  = sizeof( std::uint32_t );
    view->buf      = object.image->pixels.data();
    view->obj      = self;
    Py_INCREF( self );
    view->len      = object.shape * object.stride;
    view->readonly = 0;
    view->itemsize = sizeof( std::uint32_t );
    view->format   = (flags & PyBUF_FORMAT) != 0 ? const_cast<char *>("I") : nullptr;
    view->ndim     = 1;
    view->shape    = (flags & PyBUF_ND) == PyBUF_ND ? &object.shape : nullptr;
    view->strides  = (flags & PyBUF_STRIDES) == PyBUF_STRIDES ? &object.stride : nullptr;
    view->suboffsets = nullptr;
    view->internal   = nullptr;
    return 0;
}

PyType_Slot slots[] = {
    { Py_bf_getbuffer, reinterpret_cast<void *>(&get_buffer) },
    { 0, nullptr },
};

PyType_Spec spec = { "host.Image", sizeof( PyImage ), 0, Py_TPFLAGS_DEFAULT, slots };

/// Creates the module 'host' with the object 'image' for the image \param image of the host.
void add_module( Image &image )
{
    auto const module = add_c_api_host_module( nullptr );
    auto const type   = py::reinterpret_steal<py::object>( PyType_FromSpec( &spec ) );
    if( not type ) {
        throw py::error_already_set();
    }
    auto const object = py::reinterpret_steal<py::object>( PyType_GenericAlloc( reinterpret_cast<PyTypeObject *>(type.ptr()), 0 ) );
    if( not object ) {
        throw py::error_already_set();
    }
    reinterpret_cast<PyImage *>(object.ptr())->image = &image;
    if( PyObject_SetAttrString( module.ptr(), "image", object.ptr() ) < 0 ) {
        throw py::error_already_set();
    }
}

} // namespace c_api_buffer

/// Adds the image \param image of the host for buffer to the module \param m with pybind11: an object of the class
/// Image with the buffer protocol.
void bind_buffer( py::module_ &m, csb::host::buffer::Image &image )
{
    using csb::host::buffer::Image;
    py::class_<Image>( m, "Image", py::buffer_protocol() )
        .def_buffer( []( Image &img ) -> py::buffer_info {
            return py::buffer_info( img.pixels.data(), static_cast<py::ssize_t>(img.pixels.size()) );
        } );
    // A reference: the object of the host, not a copy.
    m.attr( "image" ) = py::cast( &image, py::return_value_policy::reference );
}

/// Adds the C++ type of cpp_objects to the module \param m with pybind11.
void bind_cpp_objects( py::module_ &m )
{
    using csb::host::cpp_objects::Vec3i;
    py::class_<Vec3i>( m, "Vec3i" )
        .def( py::init<>() )
        .def_readwrite( "x", &Vec3i::x )
        .def_readwrite( "y", &Vec3i::y )
        .def_readwrite( "z", &Vec3i::z );
}

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


/// Calls \param func with the string \param s through the C API: a str from the UTF-8 of \param s,
/// PyObject_CallOneArg(), the UTF-8 of the result. \return the result.
std::string call_c_api( PyObject *const func, std::string const &s )
{
    PyObject *const arg = PyUnicode_FromStringAndSize( s.data(), static_cast<Py_ssize_t>(s.size()) );
    if( arg == nullptr ) {
        throw py::error_already_set();
    }
    PyObject *const result = PyObject_CallOneArg( func, arg );
    Py_DECREF( arg );
    if( result == nullptr ) {
        throw py::error_already_set();
    }
    Py_ssize_t length = 0;
    char const *const chars = PyUnicode_AsUTF8AndSize( result, &length );
    if( chars == nullptr ) {
        Py_DECREF( result );
        throw py::error_already_set();
    }
    std::string value( chars, static_cast<std::size_t>(length) );
    Py_DECREF( result );
    return value;
}


/// Calls \param func with a list of the elements of \param a through the C API: PyList_New(), PyLong_FromLong(),
/// PyObject_CallOneArg(). \return the elements of the list it returns.
std::vector<int> call_c_api( PyObject *const func, std::vector<int> const &a )
{
    PyObject *const list = PyList_New( static_cast<Py_ssize_t>(a.size()) );
    if( list == nullptr ) {
        throw py::error_already_set();
    }
    for( std::size_t i = 0; i < a.size(); ++i ) {
        PyObject *const item = PyLong_FromLong( a[i] );
        if( item == nullptr ) {
            Py_DECREF( list );
            throw py::error_already_set();
        }
        PyList_SET_ITEM( list, static_cast<Py_ssize_t>(i), item );  // steals the reference
    }
    PyObject *const result = PyObject_CallOneArg( func, list );
    Py_DECREF( list );
    if( result == nullptr ) {
        throw py::error_already_set();
    }
    if( not PyList_Check( result ) ) {
        Py_DECREF( result );
        throw std::runtime_error( "f() must return a list" );
    }
    Py_ssize_t const size = PyList_GET_SIZE( result );
    std::vector<int> b;
    b.reserve( static_cast<std::size_t>(size) );
    for( Py_ssize_t i = 0; i < size; ++i ) {
        long const value = PyLong_AsLong( PyList_GET_ITEM( result, i ) );
        if( value == -1 && PyErr_Occurred() ) {
            Py_DECREF( result );
            throw py::error_already_set();
        }
        b.push_back( static_cast<int>(value) );
    }
    Py_DECREF( result );
    return b;
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
    py::object                             mF;        // the script function f of string_exchange and array_exchange
    csb::host::buffer::Image               mImage;    // buffer: the image of the host, 'host.image' in the script

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
                    (void)add_c_api_host_module( c_api_script_to_host::methods );
                } else {
                    auto m = py::module_::import( "host" );
                    bind_script_to_host( m );
                }
            } else if( info.name == "cpp_objects" ) {
                if( mVariant.c_api ) {
                    c_api_cpp_objects::add_module();
                } else {
                    auto m = py::module_::import( "host" );
                    bind_cpp_objects( m );
                }
            } else if( info.name == "buffer" ) {
                if( mVariant.c_api ) {
                    c_api_buffer::add_module( mImage );
                } else {
                    auto m = py::module_::import( "host" );
                    bind_buffer( m, mImage );
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
            } else if( mBenchmark == "string_exchange" || mBenchmark == "array_exchange" ) {
                mF = find_function( globals, "f" );
            } else {
                mBench = find_function( globals, "bench" );
            }
            mGlobals = std::move( globals );
            return phases;
        } );
    }

    std::int64_t call_bench( std::int64_t const n ) override
    {
        return translate_errors( [&]() -> std::int64_t {
            if( mBenchmark == "host_to_script" ) {
                return call_host_to_script( n );
            }
            if( mBenchmark == "array_exchange" ) {
                // pybind11: the call of the function object with the std::vector, cast<std::vector<int>>() of the result
                // (pybind11/stl.h converts to and from a list). C API: see call_c_api().
                if( mVariant.c_api ) {
                    return csb::host::array_exchange::run( n, [&]( std::vector<int> const &a ) { return call_c_api( mF.ptr(), a ); } );
                }
                return csb::host::array_exchange::run( n, [&]( std::vector<int> const &a ) { return mF( a ).cast<std::vector<int>>(); } );
            }
            if( mBenchmark == "string_exchange" ) {
                // pybind11: the call of the function object, cast<std::string>() of the result. C API: see call_c_api().
                if( mVariant.c_api ) {
                    return csb::host::string_exchange::run( n, [&]( std::string const &s ) { return call_c_api( mF.ptr(), s ); } );
                }
                return csb::host::string_exchange::run( n, [&]( std::string const &s ) { return mF( s ).cast<std::string>(); } );
            }
            if( mBenchmark == "buffer" ) {
                // The host resizes and clears its image, the script fills it, the host computes the checksum over it.
                mImage.reset( n );
                return csb::host::buffer::result( n, call_script_bench( n ), mImage.bytes() );
            }
            return call_script_bench( n );
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
        mF       = py::object();
        mGlobals = py::object();
        mInterpreter.reset();
    }

private:
    /// Calls bench( n ) of the script. \return its result.
    std::int64_t call_script_bench( std::int64_t const n )
    {
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
    }

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
