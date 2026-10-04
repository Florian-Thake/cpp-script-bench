/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include "csb/runner.hpp"
#include "json_report.hpp"
#include "stats.hpp"
#include "system_info.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <exception>
#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>


namespace csb {

namespace {

using Clock = std::chrono::steady_clock;

double seconds_between( Clock::time_point const start, Clock::time_point const end )
{
    return std::chrono::duration<double>( end - start ).count();
}


struct Options
{
    bool                         help = false;
    bool                         list = false;
    std::vector<std::string>     benchmarks;  // empty == all
    std::vector<std::string>     variants;
    std::optional<std::int64_t>  n;
    int                          warmup = 3;
    int                          runs   = 10;
    std::filesystem::path        scripts_dir = CSB_DEFAULT_SCRIPTS_DIR;
    std::filesystem::path        json_file;
};

std::vector<std::string> split( std::string_view s, char const sep )
{
    std::vector<std::string> parts;
    while( true ) {
        auto const pos = s.find( sep );
        parts.emplace_back( s.substr( 0, pos ) );
        if( pos == std::string_view::npos ) {
            return parts;
        }
        s.remove_prefix( pos + 1 );
    }
}

std::int64_t parse_int( std::string_view const option, std::string const &value, std::int64_t const min )
{
    try {
        size_t pos = 0;
        auto const v = std::stoll( value, &pos );
        if( pos == value.size() && v >= min ) {
            return v;
        }
    } catch( std::exception const & ) {
        // handled below
    }
    throw std::invalid_argument( "invalid value for " + std::string( option ) + ": " + value );
}

bool contains( std::vector<std::string> const &list, std::string const &value )
{
    return std::find( list.begin(), list.end(), value ) != list.end();
}

BenchmarkInfo const *find_benchmark( std::span<BenchmarkInfo const> const benchmarks, std::string_view const name )
{
    for( auto const &info : benchmarks ) {
        if( info.name == name ) {
            return &info;
        }
    }
    return nullptr;
}

Options parse_options( int const argc, char *argv[], LanguageEntry const &language, std::span<BenchmarkInfo const> const benchmarks )
{
    Options      opt;
    std::string  variant_arg = language.default_variant;

    for( int i = 1; i < argc; ++i ) {
        std::string const arg = argv[i];
        auto const value = [&]() -> std::string {
            if( i + 1 >= argc ) {
                throw std::invalid_argument( "missing value for " + arg );
            }
            return argv[++i];
        };

        if( arg == "--help" || arg == "-h" ) {
            opt.help = true;
        } else if( arg == "--list" ) {
            opt.list = true;
        } else if( arg == "--bench" ) {
            opt.benchmarks = split( value(), ',' );
        } else if( arg == "--variant" ) {
            variant_arg = value();
        } else if( arg == "--n" ) {
            opt.n = parse_int( arg, value(), 0 );
        } else if( arg == "--warmup" ) {
            opt.warmup = static_cast<int>(parse_int( arg, value(), 0 ));
        } else if( arg == "--runs" ) {
            opt.runs = static_cast<int>(parse_int( arg, value(), 1 ));
        } else if( arg == "--scripts" ) {
            opt.scripts_dir = value();
        } else if( arg == "--json" ) {
            opt.json_file = value();
        } else {
            throw std::invalid_argument( "unknown option: " + arg );
        }
    }

    for( auto const &name : opt.benchmarks ) {
        if( find_benchmark( benchmarks, name ) == nullptr ) {
            throw std::invalid_argument( "unknown benchmark: " + name + " (see --list)" );
        }
    }

    if( variant_arg == "all" ) {
        opt.variants = language.variants;
    } else {
        for( auto const &variant : split( variant_arg, ',' ) ) {
            if( not contains( language.variants, variant ) ) {
                throw std::invalid_argument( "unknown variant: " + variant + " (see --list)" );
            }
            opt.variants.push_back( variant );
        }
    }

    return opt;
}

void print_usage( LanguageEntry const &language )
{
    std::printf( "Usage: csb_%s [options]\n\n", language.id.c_str() );
    std::printf( "  --list               list the benchmarks and variants, then exit\n" );
    std::printf( "  --bench <a,b,...>    run only these benchmarks (default: all)\n" );
    std::printf( "  --variant <v,...>    variants to run, or 'all' (default: %s)\n", language.default_variant.empty() ? "the only one" : language.default_variant.c_str() );
    std::printf( "  --n <N>              override the workload size of all selected benchmarks\n" );
    std::printf( "  --warmup <W>         unmeasured calls after the first call (default: 3)\n" );
    std::printf( "  --runs <R>           measured calls (default: 10)\n" );
    std::printf( "  --scripts <dir>      benchmark scripts directory (default: %s)\n", CSB_DEFAULT_SCRIPTS_DIR );
    std::printf( "  --json <file>        also write the results as JSON\n" );
}

std::filesystem::path script_path( Options const &opt, BenchmarkInfo const &info, std::string const &extension )
{
    std::string const name( info.name );
    return opt.scripts_dir / name / (name + extension);
}

void print_list( LanguageEntry const &language, Options const &opt, std::span<BenchmarkInfo const> const benchmarks )
{
    auto const probe     = language.make( opt.variants.front() );
    auto const extension = probe->script_extension();

    std::printf( "Benchmarks:\n" );
    for( auto const &info : benchmarks ) {
        bool const has_script = extension.empty() || std::filesystem::exists( script_path( opt, info, extension ) );
        std::printf( "  %-20s %-8s n=%-10lld %s%s\n", std::string( info.name ).c_str(), std::string( info.group ).c_str(),
                     static_cast<long long>(info.default_n), std::string( info.description ).c_str(), has_script ? "" : "  [no script]" );
    }

    std::printf( "\nVariants:\n" );
    for( auto const &variant : language.variants ) {
        std::printf( "  %s%s\n", variant.empty() ? "(only one)" : variant.c_str(), variant == language.default_variant ? " (default)" : "" );
    }
}

std::string read_file( std::filesystem::path const &path )
{
    std::ifstream in( path, std::ios::binary );
    if( not in ) {
        throw std::runtime_error( "cannot open " + path.string() );
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

struct WrongResult {};

BenchResult run_benchmark( LanguageEntry const &language, std::string const &variant, BenchmarkInfo const &info, Options const &opt )
{
    BenchResult r;
    r.benchmark = info.name;
    r.group     = info.group;
    r.n         = opt.n.value_or( info.default_n );

    try {
        auto lang = language.make( variant );

        // All file IO happens here, before anything is measured.
        std::string source;
        auto const  extension = lang->script_extension();
        if( not extension.empty() ) {
            auto const path = script_path( opt, info, extension );
            if( not std::filesystem::exists( path ) ) {
                r.status = "n/a";
                return r;
            }
            source = read_file( path );
        }

        r.expected = info.reference( opaque( r.n ) );

        auto const t0 = Clock::now();
        lang->create_engine();
        auto const t1 = Clock::now();
        r.load_phases = lang->load( info, source );
        auto const t2 = Clock::now();
        r.startup = seconds_between( t0, t1 );
        r.load    = seconds_between( t1, t2 );

        auto const measure = [&]() -> double {
            lang->collect_garbage();
            auto const start = Clock::now();
            r.checksum = lang->call_bench( r.n );
            auto const end = Clock::now();
            if( r.checksum != r.expected ) {
                throw WrongResult{};
            }
            return seconds_between( start, end );
        };

        r.first = measure();
        for( int i = 0; i < opt.warmup; ++i ) {
            (void)measure();
        }
        r.samples.reserve( static_cast<size_t>(opt.runs) );
        for( int i = 0; i < opt.runs; ++i ) {
            r.samples.push_back( measure() );
        }

        auto const t3 = Clock::now();
        lang->destroy_engine();
        r.teardown = seconds_between( t3, Clock::now() );

        r.status = "ok";
    } catch( WrongResult const & ) {
        r.status = "wrong result";
    } catch( std::exception const &ex ) {
        r.status = std::string( "error: " ) + ex.what();
    }

    return r;
}

std::string format_time( double const secs )
{
    char buf[32];
    if( secs < 1e-3 ) {
        std::snprintf( buf, sizeof( buf ), "%.1f us", secs * 1e6 );
    } else if( secs < 1.0 ) {
        std::snprintf( buf, sizeof( buf ), "%.2f ms", secs * 1e3 );
    } else {
        std::snprintf( buf, sizeof( buf ), "%.3f s", secs );
    }
    return buf;
}

void print_result( BenchResult const &r )
{
    if( r.ok() ) {
        auto const s = compute_stats( r.samples );
        char spread[16];
        std::snprintf( spread, sizeof( spread ), "%.1f%%", s.median > 0.0 ? s.stddev / s.median * 100.0 : 0.0 );
        std::printf( "%-20s %8lld %11s %11s %11s %11s %11s %7s  ok\n", r.benchmark.c_str(), static_cast<long long>(r.n),
                     format_time( r.startup ).c_str(), format_time( r.load ).c_str(), format_time( r.first ).c_str(),
                     format_time( s.median ).c_str(), format_time( s.min ).c_str(), spread );
    } else if( r.status == "wrong result" ) {
        std::printf( "%-20s %8lld  WRONG RESULT: got %lld, expected %lld\n", r.benchmark.c_str(), static_cast<long long>(r.n),
                     static_cast<long long>(r.checksum), static_cast<long long>(r.expected) );
    } else {
        std::printf( "%-20s %8lld  %s\n", r.benchmark.c_str(), static_cast<long long>(r.n), r.status.c_str() );
    }
    std::fflush( stdout );
}

std::string utc_timestamp()
{
    std::time_t const now = std::time( nullptr );
    std::tm tm{};
#if defined( _WIN32 )
    gmtime_s( &tm, &now );
#else
    gmtime_r( &now, &tm );
#endif
    char buf[32];
    std::strftime( buf, sizeof( buf ), "%Y-%m-%dT%H:%M:%SZ", &tm );
    return buf;
}

} // namespace


int run_main( int const argc, char *argv[], LanguageEntry const &language, std::span<BenchmarkInfo const> const benchmarks )
{
    Options opt;
    try {
        opt = parse_options( argc, argv, language, benchmarks );
    } catch( std::exception const &ex ) {
        std::fprintf( stderr, "%s\n\n", ex.what() );
        print_usage( language );
        return 2;
    }

    if( opt.help ) {
        print_usage( language );
        return 0;
    }
    if( opt.list ) {
        print_list( language, opt, benchmarks );
        return 0;
    }

    Report report;
    report.language_id = language.id;
    report.system      = query_system_info();
    report.timestamp   = utc_timestamp();
    report.warmup      = opt.warmup;
    report.runs        = opt.runs;
    {
        auto const probe        = language.make( opt.variants.front() );
        report.language_name    = probe->name();
        report.language_version = probe->version();
    }

    std::printf( "cpp-script-bench %s\n", report.system.csb_version.c_str() );
    std::printf( "language : %s %s\n", report.language_name.c_str(), report.language_version.c_str() );
    std::printf( "cpu      : %s\n", report.system.cpu.c_str() );
    std::printf( "os       : %s\n", report.system.os.c_str() );
    std::printf( "compiler : %s, %s, arch %s\n", report.system.compiler.c_str(), report.system.build_config.c_str(), report.system.arch.c_str() );
    std::printf( "settings : warmup %d, runs %d\n", opt.warmup, opt.runs );
#if !defined( NDEBUG )
    std::printf( "\nWARNING: This is not an optimized build. The results are not meaningful!\n" );
#endif

    bool all_ok = true;
    for( auto const &variant : opt.variants ) {
        std::printf( "\n== %s%s%s ==\n", report.language_name.c_str(), variant.empty() ? "" : " ", variant.c_str() );
        std::printf( "%-20s %8s %11s %11s %11s %11s %11s %7s  %s\n", "benchmark", "n", "startup", "load", "first", "median", "min", "stddev", "status" );

        VariantResult vr;
        vr.variant = variant;
        for( auto const &info : benchmarks ) {
            if( not opt.benchmarks.empty() && not contains( opt.benchmarks, std::string( info.name ) ) ) {
                continue;
            }
            auto r = run_benchmark( language, variant, info, opt );
            print_result( r );
            all_ok = all_ok && (r.ok() || r.status == "n/a");
            vr.benchmarks.push_back( std::move( r ) );
        }
        report.variants.push_back( std::move( vr ) );
    }

    if( not opt.json_file.empty() ) {
        std::ofstream out( opt.json_file, std::ios::binary );
        if( not out ) {
            std::fprintf( stderr, "cannot write %s\n", opt.json_file.string().c_str() );
            return 1;
        }
        write_json( report, out );
        std::printf( "\nresults written to %s\n", opt.json_file.string().c_str() );
    }

    return all_ok ? 0 : 1;
}

} // namespace csb
