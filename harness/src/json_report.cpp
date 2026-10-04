/*
 * SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
 * SPDX-License-Identifier: MIT
 */
#include "json_report.hpp"
#include "stats.hpp"

#include <cstdio>
#include <string_view>


namespace csb {

namespace {

std::string str( std::string_view const s )
{
    std::string res = "\"";
    for( char const c : s ) {
        switch( c ) {
        case '"':  res += "\\\""; break;
        case '\\': res += "\\\\"; break;
        case '\n': res += "\\n";  break;
        case '\r': res += "\\r";  break;
        case '\t': res += "\\t";  break;
        default:
            if( static_cast<unsigned char>(c) < 0x20 ) {
                char buf[8];
                std::snprintf( buf, sizeof( buf ), "\\u%04x", static_cast<unsigned int>(c) );
                res += buf;
            } else {
                res += c;
            }
        }
    }
    res += '"';
    return res;
}

std::string num( double const v )
{
    char buf[32];
    std::snprintf( buf, sizeof( buf ), "%.9g", v );
    return buf;
}

void write_benchmark( BenchResult const &b, std::ostream &out )
{
    char const *const ind = "          ";
    out << "        {\n";
    out << ind << "\"name\": " << str( b.benchmark ) << ",\n";
    out << ind << "\"group\": " << str( b.group ) << ",\n";
    out << ind << "\"n\": " << b.n << ",\n";
    out << ind << "\"status\": " << str( b.status );
    if( b.ok() ) {
        out << ",\n" << ind << "\"checksum\": " << b.checksum << ",\n";
        out << ind << "\"startup_s\": " << num( b.startup ) << ",\n";
        out << ind << "\"load_s\": " << num( b.load ) << ",\n";
        out << ind << "\"load_phases_s\": {";
        for( size_t i = 0; i < b.load_phases.size(); ++i ) {
            out << (i == 0 ? " " : ", ") << str( b.load_phases[i].name ) << ": " << num( b.load_phases[i].seconds );
        }
        out << (b.load_phases.empty() ? "},\n" : " },\n");
        out << ind << "\"first_s\": " << num( b.first ) << ",\n";
        out << ind << "\"teardown_s\": " << num( b.teardown ) << ",\n";

        auto const s = compute_stats( b.samples );
        out << ind << "\"execute_s\": {\n";
        out << ind << "  \"min\": " << num( s.min ) << ", \"median\": " << num( s.median ) << ", \"mean\": " << num( s.mean )
            << ", \"stddev\": " << num( s.stddev ) << ", \"max\": " << num( s.max ) << ",\n";
        out << ind << "  \"samples\": [";
        for( size_t i = 0; i < b.samples.size(); ++i ) {
            out << (i == 0 ? " " : ", ") << num( b.samples[i] );
        }
        out << " ]\n" << ind << "}\n";
    } else {
        out << ",\n" << ind << "\"expected\": " << b.expected << ",\n";
        out << ind << "\"checksum\": " << b.checksum << "\n";
    }
    out << "        }";
}

} // namespace


void write_json( Report const &report, std::ostream &out )
{
    out << "{\n";
    out << "  \"schema\": 1,\n";
    out << "  \"tool\": " << str( "cpp-script-bench " + report.system.csb_version ) << ",\n";
    out << "  \"timestamp\": " << str( report.timestamp ) << ",\n";
    out << "  \"language\": { \"id\": " << str( report.language_id ) << ", \"name\": " << str( report.language_name )
        << ", \"version\": " << str( report.language_version ) << " },\n";
    out << "  \"system\": {\n";
    out << "    \"cpu\": " << str( report.system.cpu ) << ",\n";
    out << "    \"os\": " << str( report.system.os ) << ",\n";
    out << "    \"compiler\": " << str( report.system.compiler ) << ",\n";
    out << "    \"build_config\": " << str( report.system.build_config ) << ",\n";
    out << "    \"arch\": " << str( report.system.arch ) << "\n";
    out << "  },\n";
    out << "  \"settings\": { \"warmup\": " << report.warmup << ", \"runs\": " << report.runs << " },\n";
    out << "  \"variants\": [\n";
    for( size_t v = 0; v < report.variants.size(); ++v ) {
        auto const &variant = report.variants[v];
        out << "    {\n";
        out << "      \"variant\": " << str( variant.variant ) << ",\n";
        out << "      \"benchmarks\": [\n";
        for( size_t b = 0; b < variant.benchmarks.size(); ++b ) {
            write_benchmark( variant.benchmarks[b], out );
            out << (b + 1 < variant.benchmarks.size() ? ",\n" : "\n");
        }
        out << "      ]\n";
        out << "    }" << (v + 1 < report.variants.size() ? ",\n" : "\n");
    }
    out << "  ]\n";
    out << "}\n";
}

} // namespace csb
