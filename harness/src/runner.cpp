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


/// The minimum number of measured calls, also when the time budget is exceeded (unless --runs is lower).
constexpr int min_runs = 3;

struct Options
{
    bool                       help = false;
    bool                       list = false;
    std::vector<std::string>   benchmarks;  // empty == all
    std::vector<std::string>   variants;
    std::vector<std::int64_t>  n;           // empty == the series of each benchmark
    int                        warmup = 3;
    int                        runs   = 10;
    int                        budget = 30;  // seconds for all calls of bench( n ) of one run, 0 == unlimited
    std::filesystem::path      scripts_dir = CSB_DEFAULT_SCRIPTS_DIR;
    std::filesystem::path      json_file;

    int min_runs() const { return std::min( csb::min_runs, runs ); }
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
            opt.n.clear();
            for( auto const &part : split( value(), ',' ) ) {
                opt.n.push_back( parse_int( arg, part, 0 ) );
            }
        } else if( arg == "--warmup" ) {
            opt.warmup = static_cast<int>(parse_int( arg, value(), 0 ));
        } else if( arg == "--runs" ) {
            opt.runs = static_cast<int>(parse_int( arg, value(), 1 ));
        } else if( arg == "--budget" ) {
            opt.budget = static_cast<int>(parse_int( arg, value(), 0 ));
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

void print_usage( char const *const argv0, LanguageEntry const &language )
{
    std::printf( "Usage: %s [options]\n\n", std::filesystem::path( argv0 ).stem().string().c_str() );
    std::printf( "  --list               list the benchmarks and variants, then exit\n" );
    std::printf( "  --bench <a,b,...>    run only these benchmarks (default: all)\n" );
    std::printf( "  --variant <v,...>    variants to run, or 'all' (default: %s)\n", language.default_variant.empty() ? "the only one" : language.default_variant.c_str() );
    std::printf( "  --n <N,...>          override the workload sizes (series) of all selected benchmarks\n" );
    std::printf( "  --warmup <W>         unmeasured calls after the first call (default: 3)\n" );
    std::printf( "  --runs <R>           measured calls (default: 10)\n" );
    std::printf( "  --budget <S>         time budget in seconds for all calls of bench( n ) per benchmark and n,\n" );
    std::printf( "                       0 = unlimited (default: 30). At least %d calls are measured.\n", min_runs );
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
        bool const has_script = extension.empty() || not info.has_script || std::filesystem::exists( script_path( opt, info, extension ) );
        std::string series;
        for( auto const n : info.series ) {
            series += (series.empty() ? "" : ",") + std::to_string( n );
        }
        std::printf( "  %-20s %-8s n=%-10s %s%s\n", std::string( info.name ).c_str(), std::string( info.group ).c_str(),
                     series.c_str(), std::string( info.description ).c_str(), has_script ? "" : "  [no script]" );
    }

    std::printf( "\nVariants:\n" );
    for( auto const &variant : language.variants ) {
        std::string tags;
        if( variant == language.default_variant ) {
            tags = "default";
        }
        if( variant == language.headline_variant ) {
            tags += (tags.empty() ? "" : ", ") + std::string( "headline" );
        }
        std::printf( "  %s%s\n", variant.empty() ? "(no name)" : variant.c_str(), tags.empty() ? "" : (" (" + tags + ")").c_str() );
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

/// Reads the script of \param info for \param lang into \param source, if the benchmark has one.
/// \return false if the language has no script for \param info.
bool read_script( Language const &lang, BenchmarkInfo const &info, Options const &opt, std::string &source )
{
    auto const extension = lang.script_extension();
    if( extension.empty() || not info.has_script ) {
        return true;
    }
    auto const path = script_path( opt, info, extension );
    if( not std::filesystem::exists( path ) ) {
        return false;
    }
    source = read_file( path );
    return true;
}

/// A throw-away run before the first measured run of a variant: creates an engine, loads the script of \param info,
/// calls bench( n ) once with the smallest n and destroys the engine (a host API benchmark: one call of run_host()).
/// Nothing is measured or reported. It warms up what is only cold once per process (the first use of the code, the
/// libraries and their files), so the first measured run is comparable to the following ones. Errors are ignored
/// here, the measured run reports them.
/// \return false if the language has no script for \param info.
bool warm_up( LanguageEntry const &language, std::string const &variant, BenchmarkInfo const &info, Options const &opt )
{
    std::int64_t n = *std::ranges::min_element( info.series );
    for( auto const v : opt.n ) {
        n = std::min( n, v );
    }

    try {
        auto lang = language.make( variant );
        if( not lang->supports( info ) ) {
            return false;
        }
        std::string source;
        if( not read_script( *lang, info, opt, source ) ) {
            return false;
        }

        if( info.group == "host" ) {
            (void)lang->run_host( info, n, source );
            return true;
        }

        lang->create_engine();
        try {
            lang->bind( info );
            (void)lang->load( info, source );
            (void)lang->call_bench( n );
        } catch( std::exception const & ) {
            lang->destroy_engine();
            throw;
        }
        lang->destroy_engine();
    } catch( std::exception const & ) {
        // reported by the measured run
    }
    return true;
}

BenchResult run_benchmark( LanguageEntry const &language, std::string const &variant, BenchmarkInfo const &info, std::int64_t const n,
                           Options const &opt )
{
    BenchResult r;
    r.benchmark = info.name;
    r.group     = info.group;
    r.n         = n;

    try {
        auto lang = language.make( variant );
        if( not lang->supports( info ) ) {
            r.status = "n/a";
            return r;
        }

        // All file IO happens here, before anything is measured.
        std::string source;
        if( not read_script( *lang, info, opt, source ) ) {
            r.status = "n/a";
            return r;
        }

        r.expected = info.reference( opaque( r.n ) );

        // The host API benchmarks have no engine of the runner: each call of run_host() is one sample, measured by the
        // adapter itself.
        bool const host = r.host();
        if( not host ) {
            auto const t0 = Clock::now();
            lang->create_engine();
            auto const t1 = Clock::now();
            lang->bind( info );
            auto const t2 = Clock::now();
            r.load_phases = lang->load( info, source );
            auto const t3 = Clock::now();
            r.startup = seconds_between( t0, t1 );
            r.bind    = seconds_between( t1, t2 );
            r.load    = seconds_between( t2, t3 );
        }

        double last = 0.0;  // the duration of the latest call
        std::vector<Phase> phases;  // the sub-phases of the latest call of run_host()
        auto const measure = [&]() -> double {
            if( host ) {
                auto sample = lang->run_host( info, r.n, source );
                r.checksum = sample.checksum;
                last       = sample.seconds;
                phases     = std::move( sample.phases );
            } else {
                lang->collect_garbage();
                auto const start = Clock::now();
                r.checksum = lang->call_bench( r.n );
                auto const end = Clock::now();
                last = seconds_between( start, end );
            }
            if( r.checksum != r.expected ) {
                throw WrongResult{};
            }
            return last;
        };
        // Adds the sub-phases of the latest call to the result (only for measured calls).
        auto const add_phases = [&]() {
            for( auto const &phase : phases ) {
                auto it = std::ranges::find( r.phase_samples, phase.name, &PhaseSamples::name );
                if( it == r.phase_samples.end() ) {
                    it = r.phase_samples.insert( it, PhaseSamples{ phase.name, {} } );
                }
                it->samples.push_back( phase.seconds );
            }
        };

        // The time budget covers all calls, starting with the first one. A call is only made if it is expected to
        // finish within the budget (estimated with the duration of the latest call), except for the minimum
        // number of measured calls. A warm-up call also needs room for those.
        auto const calls_start = Clock::now();
        auto const fits = [&]( int const calls ) {
            return opt.budget == 0 || seconds_between( calls_start, Clock::now() ) + calls * last <= opt.budget;
        };

        r.first = measure();
        while( r.warmup < opt.warmup && fits( 1 + opt.min_runs() ) ) {
            (void)measure();
            ++r.warmup;
        }
        r.samples.reserve( static_cast<size_t>(opt.runs) );
        while( std::ssize( r.samples ) < opt.runs && (std::ssize( r.samples ) < opt.min_runs() || fits( 1 )) ) {
            r.samples.push_back( measure() );
            add_phases();
        }
        r.budget_limited = r.warmup < opt.warmup || std::ssize( r.samples ) < opt.runs;

        if( not host ) {
            auto const t4 = Clock::now();
            lang->destroy_engine();
            r.teardown = seconds_between( t4, Clock::now() );
        }

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
        // Only the interop benchmarks bind something. The host API benchmarks have no engine of the runner, but the
        // medians of their sub-phases.
        std::string const startup = r.host() ? "-" : format_time( r.startup );
        std::string const bind    = r.group == "interop" ? format_time( r.bind ) : "-";
        std::string const load    = r.host() ? "-" : format_time( r.load );
        std::string       phases;
        for( auto const &phase : r.phase_samples ) {
            phases += (phases.empty() ? "  " : ", ") + phase.name + " " + format_time( compute_stats( phase.samples ).median );
        }
        std::printf( "%-20s %8lld %11s %11s %11s %11s %5zu %11s %11s %7s  ok%s%s\n", r.benchmark.c_str(), static_cast<long long>(r.n),
                     startup.c_str(), bind.c_str(), load.c_str(), format_time( r.first ).c_str(), r.samples.size(),
                     format_time( s.median ).c_str(), format_time( s.min ).c_str(), spread, r.budget_limited ? " (time budget)" : "",
                     phases.c_str() );
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
        print_usage( argv[0], language );
        return 2;
    }

    if( opt.help ) {
        print_usage( argv[0], language );
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
    report.budget      = opt.budget;
    report.min_runs    = opt.min_runs();
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
    if( opt.budget > 0 ) {
        std::printf( "settings : warmup %d, runs %d, time budget %d s (at least %d runs)\n", opt.warmup, opt.runs, opt.budget, opt.min_runs() );
    } else {
        std::printf( "settings : warmup %d, runs %d, no time budget\n", opt.warmup, opt.runs );
    }
#if !defined( NDEBUG )
    std::printf( "\nWARNING: This is not an optimized build. The results are not meaningful!\n" );
#endif

    bool all_ok = true;
    for( auto const &variant : opt.variants ) {
        VariantResult vr;
        vr.variant  = variant;
        vr.headline = variant == language.headline_variant;

        std::printf( "\n== %s%s%s%s ==\n", report.language_name.c_str(), variant.empty() ? "" : " ", variant.c_str(), vr.headline ? " (headline)" : "" );
        std::printf( "%-20s %8s %11s %11s %11s %11s %5s %11s %11s %7s  %s\n", "benchmark", "n", "startup", "bind", "load", "first", "runs", "median",
                     "min", "stddev", "status" );

        // One throw-away run per variant, with the first selected benchmark which has a script.
        for( auto const &info : benchmarks ) {
            if( (opt.benchmarks.empty() || contains( opt.benchmarks, std::string( info.name ) )) && warm_up( language, variant, info, opt ) ) {
                break;
            }
        }

        for( auto const &info : benchmarks ) {
            if( not opt.benchmarks.empty() && not contains( opt.benchmarks, std::string( info.name ) ) ) {
                continue;
            }
            auto const series = opt.n.empty() ? info.series : std::span<std::int64_t const>( opt.n );
            for( auto const n : series ) {
                auto r = run_benchmark( language, variant, info, n, opt );
                print_result( r );
                all_ok = all_ok && (r.ok() || r.status == "n/a");
                vr.benchmarks.push_back( std::move( r ) );
            }
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
