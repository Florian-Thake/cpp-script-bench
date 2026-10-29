#!/usr/bin/env python3
# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT
"""Measures and publishes several result sets one after the other on Windows: MSVC, then GCC in WSL2.

A set takes hours. This driver is meant to be started in a terminal of its own, independent of any other session,
and to be left alone: it keeps Windows awake while it runs (the display may turn off), runs the sets with run.py
strictly one after the other, and finally publishes each finished set with report.py into results/ (without
committing). Its status goes into build/sets/<UTC time>/status.json after every step, the console output of the sets
into sets.log next to it. Ctrl+C stops it; --resume <dir> continues where it stopped.

Only the Python standard library is used (3.10 or newer). See PLAN.md, decision log 2026-10-29.
"""

import argparse
import ctypes
import datetime
import json
import subprocess
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent

_ES_CONTINUOUS = 0x80000000
_ES_SYSTEM_REQUIRED = 0x00000001


def utc_now():
    return datetime.datetime.now(datetime.timezone.utc).strftime('%Y-%m-%dT%H:%M:%SZ')


def keep_awake(on):
    """Keeps Windows from going to sleep while this thread runs (on), or allows it again."""
    flags = _ES_CONTINUOUS | (_ES_SYSTEM_REQUIRED if on else 0)
    ctypes.windll.kernel32.SetThreadExecutionState(flags)


def wsl_path(path):
    out = subprocess.run(['wsl', '-e', 'wslpath', '-a', str(path).replace('\\', '/')], capture_output=True, text=True)
    if out.returncode != 0 or not out.stdout.strip():
        sys.exit(f'wslpath failed for {path}: {out.stderr.strip()}')
    return out.stdout.strip()


def run_command(entry, settings):
    """The command line of run.py for one set: a new set, or --resume of its work directory."""
    work = entry['work']  # relative to the repository, the same path in Windows and in WSL2
    if (REPO / work / 'plan.json').exists():
        args = ['runner/run.py', '--resume', work]
    else:
        args = ['runner/run.py', f'{entry["build"]}/adapters', '--out', work, '--variants', settings['variants']]
        for option in ('processes', 'bench'):
            if settings.get(option):
                args += [f'--{option}', str(settings[option])]
    if entry['kind'] == 'windows':
        return [sys.executable] + args
    return ['wsl', '--cd', wsl_path(REPO), '-e', 'python3'] + args


def write_status(work, status):
    tmp = work / 'status.tmp'
    tmp.write_text(json.dumps(status, indent=1) + '\n', encoding='utf-8')
    tmp.replace(work / 'status.json')


def run_set(entry, settings, log):
    """Runs one set and copies its console output to the console and to log. Returns the exit code of run.py."""
    cmd = run_command(entry, settings)
    log.write(f'\n=== {utc_now()} {entry["name"]}: {" ".join(cmd)}\n')
    log.flush()
    p = subprocess.Popen(cmd, cwd=REPO, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True,
                         encoding='utf-8', errors='replace', bufsize=1)
    for line in p.stdout:
        sys.stdout.write(line)
        sys.stdout.flush()
        log.write(line)
        log.flush()
    return p.wait()


def publish(entry, log):
    results = REPO / entry['work'] / 'results.json'
    if not results.exists():
        return 'no results.json'
    out = subprocess.run([sys.executable, 'runner/report.py', str(results)], cwd=REPO, capture_output=True, text=True,
                         encoding='utf-8', errors='replace')
    text = (out.stdout + out.stderr).strip()
    log.write(f'=== {utc_now()} publish {entry["name"]}: {text}\n')
    log.flush()
    print(text)
    return 'published' if out.returncode == 0 else f'not published: {text}'


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    parser.add_argument('--sets', default='windows,wsl', help='the sets in their order (default: windows,wsl)')
    parser.add_argument('--windows-build', default='build/msvc-local', help='the build of the Windows set (default: build/msvc-local)')
    parser.add_argument('--wsl-build', default='build/gcc-local', help='the build of the WSL2 set (default: build/gcc-local)')
    parser.add_argument('--variants', choices=['all', 'headline'], default='all', help='all variants (default) or only the headline variants')
    parser.add_argument('--processes', type=int, help="run.py's --processes (default: run.py's)")
    parser.add_argument('--bench', help="run.py's --bench, for a test")
    parser.add_argument('--no-publish', action='store_true', help='do not publish the sets into results/')
    parser.add_argument('--resume', type=Path, metavar='DIR', help='continue the sets of the directory DIR')
    args = parser.parse_args()
    if sys.platform != 'win32':
        sys.exit('sets.py runs on Windows; on Linux, use run.py and report.py directly')

    if args.resume:
        work = args.resume.resolve()
        status = json.loads((work / 'status.json').read_text(encoding='utf-8'))
    else:
        stamp = datetime.datetime.now(datetime.timezone.utc).strftime('%Y-%m-%d_%H%M%S')
        work = REPO / 'build' / 'sets' / stamp
        work.mkdir(parents=True)
        builds = {'windows': args.windows_build, 'wsl': args.wsl_build}
        entries = []
        for kind in [k for k in args.sets.split(',') if k]:
            if kind not in builds:
                parser.error(f'unknown set: {kind} (windows or wsl)')
            entries.append({'name': kind, 'kind': kind, 'build': builds[kind],
                            'work': f'{builds[kind]}/results/{stamp}', 'state': 'pending'})
        status = {'created': utc_now(), 'settings': {'variants': args.variants, 'processes': args.processes,
                                                     'bench': args.bench, 'publish': not args.no_publish},
                  'sets': entries}
    write_status(work, status)
    print(f'status: {work / "status.json"}')

    keep_awake(True)
    try:
        with open(work / 'sets.log', 'a', encoding='utf-8') as log:
            for entry in status['sets']:
                if entry['state'] == 'done':
                    continue
                entry.update(state='running', started=utc_now())
                write_status(work, status)
                code = run_set(entry, status['settings'], log)
                entry.update(state='done' if code == 0 else 'failed', finished=utc_now(), exit_code=code)
                write_status(work, status)
                if code not in (0, 1):  # 1: finished, but with problems or incomplete (see its results.json)
                    print(f'{entry["name"]}: run.py ended with {code}; continue with --resume {work}')
                    return code
            if status['settings']['publish']:
                for entry in status['sets']:
                    if 'published' not in entry:
                        entry['published'] = publish(entry, log)
                        write_status(work, status)
        status['finished'] = utc_now()
        write_status(work, status)
    except KeyboardInterrupt:
        for entry in status['sets']:
            if entry['state'] == 'running':
                entry['state'] = 'interrupted'
        write_status(work, status)
        print(f'\ninterrupted; continue with: python runner/sets.py --resume {work}')
        return 130
    finally:
        keep_awake(False)
    print(f'done: {work / "status.json"}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
