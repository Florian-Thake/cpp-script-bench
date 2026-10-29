#!/usr/bin/env python3
# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT
"""Measures one result set: every benchmark of every runner, each run in processes of its own.

A run is one variant, one benchmark and one n. Every run is repeated in several processes (--processes). The
repetitions are interleaved: all languages measure one benchmark and n before the next one starts, and the order of
the languages rotates from row to row, so slow changes of the machine during the hours of a set affect all languages
alike. The processes are pinned to one logical CPU (--cpu) and run with the high priority class on Windows.

The output goes into a work directory:
  plan.json                the settings and the runners
  processes/<key>.json     the JSON of the runner for one process, plus what was measured from the outside:
                           wall time, CPU time, peak memory, the load of the machine during the process
  logs/<key>.log           the console output of the process
  results.json             the merged set (written when all processes are done, or with --merge)

An interrupted set is continued with --resume <work dir>. Only the Python standard library is used (3.10 or newer).
See PLAN.md, decision log 2026-10-29.
"""

import argparse
import datetime
import json
import os
import re
import subprocess
import sys
import threading
import time
from pathlib import Path

SCHEMA = 1
WINDOWS = sys.platform == 'win32'

# The order of the runners in a row before the rotation; other runners follow in alphabetical order.
RUNNER_ORDER = ['csb_cpp', 'csb_teascript', 'csb_chaiscript_nothreads', 'csb_chaiscript', 'csb_angelscript_nothreads',
                'csb_angelscript', 'csb_lua', 'csb_python']


# === measuring from the outside ===

if WINDOWS:
    import ctypes
    from ctypes import wintypes

    _k32 = ctypes.WinDLL('kernel32', use_last_error=True)
    _k32.GetCurrentProcess.restype = wintypes.HANDLE
    _k32.SetProcessAffinityMask.argtypes = [wintypes.HANDLE, ctypes.c_size_t]
    _k32.OpenProcess.restype = wintypes.HANDLE
    _k32.OpenProcess.argtypes = [wintypes.DWORD, wintypes.BOOL, wintypes.DWORD]
    _k32.CloseHandle.argtypes = [wintypes.HANDLE]
    _k32.GetSystemTimes.argtypes = [ctypes.POINTER(wintypes.FILETIME)] * 3
    _k32.GetProcessTimes.argtypes = [wintypes.HANDLE] + [ctypes.POINTER(wintypes.FILETIME)] * 4
    _k32.K32GetProcessMemoryInfo.argtypes = [wintypes.HANDLE, ctypes.c_void_p, wintypes.DWORD]

    _PROCESS_VM_READ = 0x0010
    _PROCESS_QUERY_LIMITED_INFORMATION = 0x1000

    class _MemoryCounters(ctypes.Structure):  # PROCESS_MEMORY_COUNTERS
        _fields_ = [('cb', wintypes.DWORD), ('PageFaultCount', wintypes.DWORD),
                    ('PeakWorkingSetSize', ctypes.c_size_t), ('WorkingSetSize', ctypes.c_size_t),
                    ('QuotaPeakPagedPoolUsage', ctypes.c_size_t), ('QuotaPagedPoolUsage', ctypes.c_size_t),
                    ('QuotaPeakNonPagedPoolUsage', ctypes.c_size_t), ('QuotaNonPagedPoolUsage', ctypes.c_size_t),
                    ('PagefileUsage', ctypes.c_size_t), ('PeakPagefileUsage', ctypes.c_size_t)]

    def _seconds(ft):
        return ((ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 1e7


def system_cpu_times():
    """Returns (busy, steal): the busy time of all logical CPUs together and the time the hypervisor took from them
    (Linux only, else 0), in seconds; None if unknown."""
    if WINDOWS:
        idle, kernel, user = wintypes.FILETIME(), wintypes.FILETIME(), wintypes.FILETIME()
        if not _k32.GetSystemTimes(ctypes.byref(idle), ctypes.byref(kernel), ctypes.byref(user)):
            return None
        return _seconds(kernel) + _seconds(user) - _seconds(idle), 0.0  # the kernel time includes the idle time
    try:
        with open('/proc/stat') as f:
            values = [int(v) for v in f.readline().split()[1:]]
    except OSError:
        return None
    hz = os.sysconf('SC_CLK_TCK')
    # user nice system idle iowait irq softirq steal ... (the guest time is part of user)
    busy = values[0] + values[1] + values[2] + values[5] + values[6]
    steal = values[7] if len(values) > 7 else 0
    return busy / hz, steal / hz


def pin_to_cpu(cpu):
    """Pins this process to the logical CPU cpu. The processes it starts inherit it."""
    if WINDOWS:
        if not _k32.SetProcessAffinityMask(_k32.GetCurrentProcess(), 1 << cpu):
            raise OSError(ctypes.get_last_error(), f'SetProcessAffinityMask failed for CPU {cpu}')
    else:
        os.sched_setaffinity(0, {cpu})


def power_setting():
    """The power plan (Windows) or the frequency governor of the CPUs (Linux), if known."""
    if WINDOWS:
        try:
            out = subprocess.run(['powercfg', '/getactivescheme'], capture_output=True, text=True, errors='replace').stdout
        except OSError:
            return ''
        m = re.search(r'([0-9a-fA-F-]{36})\s+\((.*)\)', out)
        return f'{m[2]} ({m[1]})' if m else out.strip()
    try:
        return 'governor ' + Path('/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor').read_text().strip()
    except OSError:
        return ''


def idle_load(seconds=2.0):
    """The number of busy logical CPUs on average over the next seconds."""
    before = system_cpu_times()
    time.sleep(seconds)
    after = system_cpu_times()
    return (after[0] - before[0]) / seconds if before and after else None


def run_process(cmd, log, timeout, high_priority):
    """Runs cmd with its output in the open file log. Returns what was measured from the outside."""
    before = system_cpu_times()
    res = {'start': utc_now()}
    cpu = peak = commit = None
    timed_out = []
    t0 = time.perf_counter()
    if WINDOWS:
        p = subprocess.Popen(cmd, stdout=log, stderr=subprocess.STDOUT,
                             creationflags=subprocess.HIGH_PRIORITY_CLASS if high_priority else 0)
        # Popen keeps its own handle until the process is waited for, so the process object still exists here.
        handle = _k32.OpenProcess(_PROCESS_QUERY_LIMITED_INFORMATION | _PROCESS_VM_READ, False, p.pid)
        try:
            try:
                p.wait(timeout=timeout)
            except subprocess.TimeoutExpired:
                timed_out.append(True)
                p.kill()
                p.wait()
            wall = time.perf_counter() - t0
            if handle:
                times = [wintypes.FILETIME() for _ in range(4)]
                if _k32.GetProcessTimes(handle, *(ctypes.byref(t) for t in times)):
                    cpu = _seconds(times[2]) + _seconds(times[3])
                mem = _MemoryCounters()
                mem.cb = ctypes.sizeof(mem)
                if _k32.K32GetProcessMemoryInfo(handle, ctypes.byref(mem), mem.cb):
                    peak, commit = mem.PeakWorkingSetSize, mem.PeakPagefileUsage
        finally:
            if handle:
                _k32.CloseHandle(handle)
        exit_code = p.returncode
    else:
        p = subprocess.Popen(cmd, stdout=log, stderr=subprocess.STDOUT)

        def kill():
            timed_out.append(True)
            p.kill()

        timer = threading.Timer(timeout, kill)
        timer.start()
        try:
            _, status, usage = os.wait4(p.pid, 0)  # unlike Popen.wait(), it returns the resource usage of the process
        finally:
            timer.cancel()
        wall = time.perf_counter() - t0
        p.returncode = exit_code = os.waitstatus_to_exitcode(status)
        cpu = usage.ru_utime + usage.ru_stime
        peak = usage.ru_maxrss * 1024
    after = system_cpu_times()

    res['wall_s'] = round(wall, 6)
    res['exit_code'] = exit_code
    if timed_out:
        res['timeout'] = True
    if cpu is not None:
        res['cpu_s'] = round(cpu, 6)
    if peak is not None:
        res['peak_memory_bytes'] = peak  # the peak working set (Windows) or resident set (Linux)
    if commit is not None:
        res['peak_commit_bytes'] = commit  # the peak private memory (Windows)
    if before and after and wall > 0:
        busy = after[0] - before[0]
        res['load'] = {'busy_cpus': round(busy / wall, 3),
                       'other_cpus': round(max(0.0, busy - (cpu or 0.0)) / wall, 3)}  # all processes but this one
        if not WINDOWS:
            res['load']['steal_cpus'] = round((after[1] - before[1]) / wall, 3)
    return res


# === runners and plan ===

def utc_now():
    return datetime.datetime.now(datetime.timezone.utc).strftime('%Y-%m-%dT%H:%M:%SZ')


def find_runners(adapters, config):
    if WINDOWS:
        paths = list(adapters.glob(f'*/{config}/csb_*.exe'))
    else:
        paths = [p for p in adapters.glob('*/csb_*') if p.is_file() and not p.suffix and os.access(p, os.X_OK)]

    def order(p):
        return (RUNNER_ORDER.index(p.stem) if p.stem in RUNNER_ORDER else len(RUNNER_ORDER), p.stem)

    return sorted(paths, key=order)


_LIST_BENCHMARK = re.compile(r'^  (\S+)\s+(\S+)\s+n=(\S+)\s')


def list_runner(exe):
    """Returns the benchmarks and the variants of the runner exe, from its --list."""
    out = subprocess.run([str(exe), '--list'], capture_output=True, text=True, check=True).stdout
    benchmarks, variants, section = [], [], None
    for line in out.splitlines():
        if line in ('Benchmarks:', 'Variants:'):
            section = line
        elif line.startswith('  ') and section == 'Benchmarks:':
            m = _LIST_BENCHMARK.match(line)
            if m:
                benchmarks.append({'name': m[1], 'group': m[2], 'series': [int(v) for v in m[3].split(',')],
                                   'script': not line.endswith('[no script]')})
        elif line.startswith('  ') and section == 'Variants:':
            text = line.strip()
            if text.startswith('(no name)'):
                name, rest = '', text[len('(no name)'):]
            else:
                name, _, rest = text.partition(' ')
            tags = rest.strip(' ()').split(', ')
            variants.append({'name': name, 'default': 'default' in tags, 'headline': 'headline' in tags})
    return benchmarks, variants


def process_key(rep, runner, variant, benchmark, n):
    return f'p{rep + 1}.{benchmark}.{n}.{runner}.{variant or "_"}'


def make_plan(args, adapters):
    runners = []
    for exe in find_runners(adapters, args.config):
        benchmarks, variants = list_runner(exe)
        if args.variants == 'headline':
            variants = [v for v in variants if v['headline']]
        if variants:
            runners.append({'runner': exe.stem, 'path': str(exe), 'benchmarks': benchmarks, 'variants': variants})
    if not runners:
        sys.exit(f'no runners found in {adapters}')
    if args.runners:
        unknown = set(args.runners) - {r['runner'] for r in runners}
        if unknown:
            sys.exit(f'unknown runners: {", ".join(sorted(unknown))}')
        runners = [r for r in runners if r['runner'] in args.runners]

    # The rows of the set: every benchmark and n, in the order of the registry.
    rows = []
    for r in runners:
        for b in r['benchmarks']:
            if args.bench and b['name'] not in args.bench:
                continue
            for n in args.n or b['series']:
                if [b['name'], n] not in rows:
                    rows.append([b['name'], n])
    if args.bench:
        unknown = set(args.bench) - {row[0] for row in rows}
        if unknown:
            sys.exit(f'unknown benchmarks: {", ".join(sorted(unknown))}')
    return {
        'schema': SCHEMA,
        'created': utc_now(),
        'adapters': str(adapters),
        'settings': {
            'processes': args.processes,
            'variants': args.variants,
            'cpu': args.cpu,
            'priority': args.priority,
            'timeout_s': args.timeout,
            'warmup': args.warmup,
            'runs': args.runs,
            'budget_s': args.budget,
            'allow_dirty': args.allow_dirty,
        },
        'runners': runners,
        'rows': rows,
    }


def plan_items(plan):
    """All processes of the plan in their order: repetition, row, then the runs of the row with a rotating order."""
    lanes = [(r, v) for r in plan['runners'] for v in r['variants']]
    for rep in range(plan['settings']['processes']):
        for index, (benchmark, n) in enumerate(plan['rows']):
            shift = (index + rep) % len(lanes)
            for runner, variant in lanes[shift:] + lanes[:shift]:
                if any(b['name'] == benchmark for b in runner['benchmarks']):
                    yield rep, runner, variant, benchmark, n


def runner_command(plan, runner, variant, benchmark, n, json_file):
    s = plan['settings']
    cmd = [runner['path'], '--bench', benchmark, '--n', str(n), '--json', str(json_file)]
    if not variant['default']:
        cmd += ['--variant', variant['name']]
    for option, value in (('--warmup', s['warmup']), ('--runs', s['runs']), ('--budget', s['budget_s'])):
        if value is not None:
            cmd += [option, str(value)]
    return cmd


# === running ===

def format_duration(seconds):
    seconds = int(seconds)
    return f'{seconds // 3600}:{seconds // 60 % 60:02d}:{seconds % 60:02d}'


def check_report(report, plan, seen):
    """Stops the set if a process reports another build than the ones before, or one that must not be published."""
    tool = report['tool']
    if seen.setdefault('tool', tool) != tool:
        sys.exit(f'the runners were built from different commits: {seen["tool"]} and {tool}')
    if report['system'].get('build_config') != 'Release':
        sys.exit(f'not a Release build: {report["language"]["id"]} ({report["system"].get("build_config")})')
    if not plan['settings']['allow_dirty']:
        version = report['language']['version']
        if '-dirty' in tool or '-dirty' in version:
            sys.exit(f'built from a modified checkout ({tool}; {report["language"]["id"]} {version}); '
                     'commit first, or use --allow-dirty for a test')


def run_set(work, plan):
    processes = work / 'processes'
    logs = work / 'logs'
    processes.mkdir(parents=True, exist_ok=True)
    logs.mkdir(exist_ok=True)
    settings = plan['settings']
    if settings['cpu'] is not None:
        pin_to_cpu(settings['cpu'])

    # A resumed set runs the crashed processes again: Ctrl+C may have ended the process before the orchestrator.
    # The runs which are "n/a" in a finished process are not repeated.
    done_keys, not_available = set(), set()
    for f in processes.glob('*.json'):
        record = json.loads(f.read_text(encoding='utf-8'))
        if not record['status'].startswith('crash'):
            done_keys.add(record['key'])
        if record['status'] == 'n/a':
            not_available.add((record['runner'], record['variant'], record['benchmark'], record['n']))
    plan.setdefault('sessions', []).append(utc_now())
    (work / 'plan.json').write_text(json.dumps(plan, indent=1), encoding='utf-8')

    items = list(plan_items(plan))
    load = idle_load()
    print(f'{len(items)} processes planned, work directory {work}')
    print(f'CPU {settings["cpu"] if settings["cpu"] is not None else "not pinned"}, priority {settings["priority"]}, '
          f'power {power_setting() or "unknown"}')
    if load is not None:
        print(f'load before the start: {load:.2f} busy logical CPUs')
    sys.stdout.flush()

    seen = {}
    start = time.perf_counter()
    done = measured = 0
    for i, (rep, runner, variant, benchmark, n) in enumerate(items):
        key = process_key(rep, runner['runner'], variant['name'], benchmark, n)
        target = processes / f'{key}.json'
        run = (runner['runner'], variant['name'], benchmark, n)
        if key in done_keys or run in not_available:
            continue

        json_file = processes / f'{key}.runner.json'
        json_file.unlink(missing_ok=True)
        cmd = runner_command(plan, runner, variant, benchmark, n, json_file)
        with open(logs / f'{key}.log', 'w', encoding='utf-8') as log:
            outside = run_process(cmd, log, settings['timeout_s'], settings['priority'] == 'high')

        record = {'key': key, 'repetition': rep + 1, 'runner': runner['runner'], 'variant': variant['name'],
                  'headline': variant['headline'], 'benchmark': benchmark, 'n': n, 'process': outside}
        try:
            report = json.loads(json_file.read_text(encoding='utf-8'))
            result = report['variants'][0]['benchmarks'][0]
            if (len(report['variants']) != 1 or len(report['variants'][0]['benchmarks']) != 1
                    or result['name'] != benchmark or result['n'] != n):
                sys.exit(f'{key}: unexpected content of {json_file}')
            if result['status'] != 'n/a':
                check_report(report, plan, seen)
            record['status'] = result['status']
            record['report'] = report
        except (OSError, ValueError, KeyError, IndexError):
            record['status'] = 'timeout' if outside.get('timeout') else f'crash (exit code {outside["exit_code"]})'
        json_file.unlink(missing_ok=True)
        tmp = target.with_suffix('.tmp')
        tmp.write_text(json.dumps(record, indent=1), encoding='utf-8')
        tmp.replace(target)
        if record['status'] == 'n/a':
            not_available.add(run)

        done += 1
        measured += record['status'] != 'n/a'
        elapsed = time.perf_counter() - start
        remaining = elapsed / done * (len(items) - i - 1)  # a rough estimate: the runs differ a lot
        # The load of short processes is noise: Windows counts the CPU times in ticks of 15.6 ms.
        other = outside.get('load', {}).get('other_cpus') if outside['wall_s'] >= 1.0 else None
        print(f'[{i + 1:5d}/{len(items)}] {format_duration(elapsed)} (~{format_duration(remaining)} left)  p{rep + 1} {benchmark} n={n} '
              f'{runner["runner"]}{" " + variant["name"] if variant["name"] else ""}: {record["status"]}, '
              f'{outside["wall_s"]:.1f} s' + (f', other load {other:.2f}' if other is not None else ''))
        sys.stdout.flush()

    print(f'{done} processes run in {format_duration(time.perf_counter() - start)}, {measured} of them measured')


# === merging ===

_NUMBER_LIST = re.compile(r'\[\s*([-+0-9.eE,\s]*?)\s*\]')


def to_json(value):
    """JSON with an indent, but every list of numbers (the samples) on one line."""
    return _NUMBER_LIST.sub(lambda m: '[' + re.sub(r'\s+', ' ', m[1]) + ']', json.dumps(value, indent=1)) + '\n'


def end_time(process):
    start = datetime.datetime.strptime(process['start'], '%Y-%m-%dT%H:%M:%SZ')
    return (start + datetime.timedelta(seconds=process['wall_s'])).strftime('%Y-%m-%dT%H:%M:%SZ')


def merge(work, plan):
    """Merges all processes of the work directory into one result set, results.json. Returns the problems found."""
    records = {}
    for f in (work / 'processes').glob('*.json'):
        record = json.loads(f.read_text(encoding='utf-8'))
        records[record['key']] = record

    problems = []
    system = tool = settings = None
    languages = {}
    runs = {}
    order = []
    absent = []
    for rep, runner, variant, benchmark, n in plan_items(plan):
        run_key = (runner['runner'], variant['name'], benchmark, n)
        key = process_key(rep, *run_key)
        record = records.get(key)
        if run_key not in runs:
            group = next(b['group'] for b in runner['benchmarks'] if b['name'] == benchmark)
            runs[run_key] = {'runner': runner['runner'], 'variant': variant['name'], 'headline': variant['headline'],
                             'benchmark': benchmark, 'group': group, 'n': n, 'processes': []}
            order.append(run_key)
        if record is None:
            absent.append((run_key, key))
            continue
        if record['status'] not in ('ok', 'n/a'):
            problems.append(f'{record["key"]}: {record["status"]}')
        process = {'repetition': record['repetition'], **record['process'], 'status': record['status']}
        report = record.get('report')
        if report and record['status'] != 'n/a':
            if system is None:
                system, tool = report['system'], report['tool']
                settings = report['settings']
            elif report['system'] != system or report['tool'] != tool or report['settings'] != settings:
                problems.append(f'{record["key"]}: another system, build or settings than the other processes')
            language = report['language']
            entry = languages.setdefault(runner['runner'], {'runner': runner['runner'], **language, 'variants': []})
            if variant['name'] not in [v['variant'] for v in entry['variants']]:
                entry['variants'].append({'variant': variant['name'], 'headline': variant['headline']})
            result = dict(report['variants'][0]['benchmarks'][0])
            for k in ('name', 'group', 'n', 'status'):
                result.pop(k, None)
            process['result'] = result
        runs[run_key]['processes'].append(process)

    # Complete: every planned process is there, except the repetitions of runs which are "n/a".
    not_available = {k for k, r in runs.items() if any(p['status'] == 'n/a' for p in r['processes'])}
    missing = [key for run_key, key in absent if run_key not in not_available]

    merged = []
    for run_key in order:
        run = runs[run_key]
        statuses = [p['status'] for p in run['processes']]
        if not statuses:
            continue
        if 'n/a' in statuses:
            run['status'] = 'n/a'
        else:
            run['status'] = next((s for s in statuses if s != 'ok'), 'ok')
        merged.append(run)

    s = plan['settings']
    result_set = {
        'schema': SCHEMA,
        'kind': 'cpp-script-bench result set',
        'tool': tool,
        'started': min((p['start'] for r in merged for p in r['processes']), default=None),
        'finished': max((end_time(p) for r in merged for p in r['processes']), default=None),
        'complete': not missing,
        'sessions': len(plan.get('sessions', [])),  # more than 1 if the set was interrupted and resumed
        'system': system,
        'power': plan.get('power', ''),
        'settings': {
            'processes': s['processes'], 'order': 'interleaved, rotating', 'cpu': s['cpu'], 'priority': s['priority'],
            'runner': settings,
        },
        'languages': list(languages.values()),
        'runs': merged,
        'problems': problems,
    }
    if '-dirty' in (tool or '') or any('-dirty' in lang['version'] for lang in languages.values()):
        result_set['dirty'] = True
    tmp = work / 'results.tmp'
    tmp.write_text(to_json(result_set), encoding='utf-8')
    tmp.replace(work / 'results.json')
    return result_set


# === command line ===

def comma_list(value):
    return [v for v in value.split(',') if v]


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    parser.add_argument('adapters', nargs='?', type=Path, help='the adapters directory of a build, e.g. build/msvc-local/adapters')
    parser.add_argument('--out', type=Path, help='the work directory (default: <build>/results/<UTC time>)')
    parser.add_argument('--resume', type=Path, metavar='DIR', help='continue the set in the work directory DIR')
    parser.add_argument('--merge', type=Path, metavar='DIR', help='only merge the processes in DIR into results.json')
    parser.add_argument('--config', default='Release', help='the configuration directory of a Visual Studio build (default: Release)')
    parser.add_argument('--runners', type=comma_list, help='only these runners, e.g. csb_lua,csb_python')
    parser.add_argument('--bench', type=comma_list, help='only these benchmarks')
    parser.add_argument('--n', type=lambda v: [int(x) for x in comma_list(v)], help='replace the series of all benchmarks')
    parser.add_argument('--variants', choices=['all', 'headline'], default='all', help='all variants (default) or only the headline variants')
    parser.add_argument('--processes', type=int, default=3, help='processes per run (default: 3)')
    parser.add_argument('--warmup', type=int, help="the runners' --warmup (default: theirs)")
    parser.add_argument('--runs', type=int, help="the runners' --runs (default: theirs)")
    parser.add_argument('--budget', type=int, help="the runners' --budget in seconds (default: theirs)")
    parser.add_argument('--cpu', default='last', help="the logical CPU for all processes, 'last' (default) or 'none'")
    parser.add_argument('--priority', choices=['high', 'normal'], help='process priority (default: high on Windows; Linux: only normal)')
    parser.add_argument('--timeout', type=int, default=1800, help='seconds until a process is killed (default: 1800)')
    parser.add_argument('--allow-dirty', action='store_true', help='allow builds of modified checkouts (for tests; the set is marked)')
    parser.add_argument('--dry-run', action='store_true', help='only print the plan')
    args = parser.parse_args()

    if args.merge:
        plan = json.loads((args.merge / 'plan.json').read_text(encoding='utf-8'))
        result_set = merge(args.merge, plan)
        print(f'{len(result_set["runs"])} runs merged into {args.merge / "results.json"}, '
              f'complete: {result_set["complete"]}, problems: {len(result_set["problems"])}')
        return 0

    if args.resume:
        work = args.resume
        plan = json.loads((work / 'plan.json').read_text(encoding='utf-8'))
    else:
        if args.adapters is None:
            parser.error('the adapters directory is missing')
        adapters = args.adapters.resolve()
        if args.priority is None:
            args.priority = 'high' if WINDOWS else 'normal'
        elif args.priority == 'high' and not WINDOWS:
            parser.error('--priority high needs root on Linux')
        threads = os.cpu_count() or 1
        if args.cpu == 'none':
            args.cpu = None
        else:
            args.cpu = threads - 1 if args.cpu == 'last' else int(args.cpu)
            if not 0 <= args.cpu < threads:
                parser.error(f'--cpu must be between 0 and {threads - 1}')
        plan = make_plan(args, adapters)
        plan['power'] = power_setting()
        if args.dry_run:
            items = list(plan_items(plan))
            for rep, runner, variant, benchmark, n in items[:40]:
                print(process_key(rep, runner['runner'], variant['name'], benchmark, n))
            print(f'... {len(items)} processes ({len(plan["rows"])} rows, '
                  f'{sum(len(r["variants"]) for r in plan["runners"])} variants, {args.processes} processes per run)')
            return 0
        stamp = datetime.datetime.now(datetime.timezone.utc).strftime('%Y-%m-%d_%H%M%S')
        work = args.out or adapters.parent / 'results' / stamp
        work.mkdir(parents=True, exist_ok=False)
        (work / 'plan.json').write_text(json.dumps(plan, indent=1), encoding='utf-8')

    try:
        run_set(work, plan)
    except KeyboardInterrupt:
        print(f'\ninterrupted; continue with --resume {work}')
        return 130
    result_set = merge(work, plan)
    print(f'results: {work / "results.json"} ({len(result_set["runs"])} runs, complete: {result_set["complete"]})')
    for problem in result_set['problems']:
        print(f'  problem: {problem}')
    return 1 if result_set['problems'] or not result_set['complete'] else 0


if __name__ == '__main__':
    sys.exit(main())
