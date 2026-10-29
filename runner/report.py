#!/usr/bin/env python3
# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT
"""Publishes a result set: a README.md with the setup, tables and charts (SVG, light and dark), readable on GitHub.

Reads the results.json of a work directory of run.py and writes into results/<set>/: README.md, charts/*.svg and a
copy of results.json. Then it writes the index results/README.md, which lists all sets.

The value of a run is the median of the medians of its processes; its spread is the range of the process medians
(slowest - fastest) relative to that median. Only the Python standard library is used (3.10 or newer).
See PLAN.md, decision log 2026-10-29.
"""

import argparse
import datetime
import json
import math
import re
import shutil
import statistics
import sys
from pathlib import Path

GROUPS = [('core', 'Core'), ('data', 'Data'), ('interop', 'Interop (script <-> C++)'), ('host', 'Host API (C++ level)')]
REFERENCE = 'cpp'

# The reference palette of the dataviz method (validated for neighbouring slots in both modes). The script languages
# take the slots in alphabetical order, a neutral order, and keep them in every chart; C++ is the gray reference mark.
SLOTS = {'light': ['#2a78d6', '#eb6834', '#1baf7a', '#eda100', '#e87ba4', '#008300', '#6250d6', '#e34948'],
         'dark': ['#3987e5', '#d95926', '#199e70', '#c98500', '#d55181', '#008300', '#9085e9', '#e66767']}
THEMES = {
    'light': {'surface': '#fcfcfb', 'primary': '#0b0b0b', 'secondary': '#52514e', 'muted': '#898781',
              'grid': '#e1e0d9', 'axis': '#c3c2b7', 'border': 'rgba(11,11,11,0.10)'},
    'dark': {'surface': '#1a1a19', 'primary': '#f0efec', 'secondary': '#c3c2b7', 'muted': '#898781',
             'grid': '#2c2c2a', 'axis': '#383835', 'border': 'rgba(255,255,255,0.10)'},
}
FONT = 'system-ui, -apple-system, &quot;Segoe UI&quot;, sans-serif'


# === formatting ===

def fmt_time(seconds):
    for unit, factor in (('s', 1.0), ('ms', 1e-3), ('µs', 1e-6)):
        if seconds >= factor:
            break
    else:
        unit, factor = 'ns', 1e-9
    v = seconds / factor
    return f'{v:.3g} {unit}' if v < 1000 else f'{v:.0f} {unit}'


def fmt_factor(f):
    return f'{f:.3g}×' if f < 1000 else f'{f:,.0f}×'


def fmt_n(n):
    return f'{n:,}'


def fmt_bytes(b):
    return f'{b / 2 ** 30:.1f} GiB'


def fmt_mib(b, sign=False):
    return f'{b / 2 ** 20:{"+" if sign else ""}.1f} MiB'


def short_cpu(cpu):
    s = re.sub(r'\(R\)|\(TM\)|@.*$|\d+-Core Processor|\bProcessor\b|\bCPU\b|\d+(st|nd|rd|th) Gen|\bIntel\b|\bAMD\b|\bCore\b',
               ' ', cpu)
    return ' '.join(s.split()) or cpu


def short_compiler(compiler):
    m = re.match(r'(MSVC|GCC|Clang) (\d+\.\d+)', compiler)
    return f'{m[1]} {m[2]}' if m else compiler


def virtualization(system):
    hv = system.get('hypervisor', '')
    if not hv:
        return 'none detected'
    if 'root partition' in hv:
        return 'Hyper-V is enabled: this OS runs in its root partition'
    if 'WSL2' in system.get('os', ''):
        return f'WSL2 (a Hyper-V VM, {system["cpu_threads"]} virtual CPUs)'
    return f'a virtual machine ({hv}, {system["cpu_threads"]} virtual CPUs)'


def default_name(system):
    compiler = short_compiler(system['compiler']).replace(' ', '').lower()
    cpu = short_cpu(system['cpu']).replace(' ', '-').lower()
    if system['os'].startswith('Windows'):
        m = re.match(r'(Windows \d+)', system.get('os_name', ''))
        os_part = (m[1] if m else 'windows').replace(' ', '').lower()
    else:
        m = re.match(r'(\w+) ([\d.]+)', system.get('os_name', ''))
        os_part = f'{m[1]}{".".join(m[2].split(".")[:2])}'.lower() if m else 'linux'
        if 'WSL2' in system['os']:
            os_part += '-wsl2'
        elif system.get('hypervisor'):
            os_part += '-vm'
    return f'{compiler}-{cpu}-{os_part}'


def setup_label(result_set):
    """The short label of a set in the footer of its charts."""
    system = result_set['system']
    return (f'{short_cpu(system["cpu"])} · {system.get("os_name") or system["os"]} · {system["compiler"]} · '
            f'{result_set["tool"].replace("cpp-script-bench ", "csb ")} · {(result_set["started"] or "")[:10]}')


# === data ===

class Lane:
    """One language variant of the set."""

    def __init__(self, language, variant, headline):
        self.id = language['id']
        self.name = language['name']
        self.runner = language['runner']
        self.version = language['version']
        self.variant = variant
        self.headline = headline

    @property
    def label(self):
        return f'{self.name} {self.variant}' if self.variant else self.name


class Value:
    """The value of one run: the median and the spread of the medians of its processes."""

    def __init__(self, run):
        self.status = run['status']
        medians = [p['result']['execute_s']['median'] for p in run['processes']
                   if p['status'] == 'ok' and 'result' in p]
        self.medians = medians
        self.median = statistics.median(medians) if medians and self.status == 'ok' else None
        self.spread = (max(medians) - min(medians)) / self.median if self.median else None
        self.budget_limited = any(p.get('result', {}).get('budget_limited') for p in run['processes'])
        # The peak memory of the processes: of the measured ones, and of all (C++ is the baseline also where it
        # is "n/a": its process is then the harness alone).
        peaks = [p['peak_memory_bytes'] for p in run['processes'] if p['status'] == 'ok' and 'peak_memory_bytes' in p]
        self.peak = statistics.median(peaks) if peaks and self.ok else None
        any_peaks = [p['peak_memory_bytes'] for p in run['processes'] if 'peak_memory_bytes' in p]
        self.peak_any = statistics.median(any_peaks) if any_peaks else None

    @property
    def ok(self):
        return self.median is not None


class ResultSet:
    def __init__(self, data):
        self.data = data
        self.system = data['system']
        self.lanes = []
        self.slot = {}  # language id -> categorical slot
        languages = {lang['runner']: lang for lang in data['languages']}
        for lang in data['languages']:
            for v in lang['variants']:
                self.lanes.append(Lane(lang, v['variant'], v['headline']))
        script_ids = sorted({lane.id for lane in self.lanes if lane.id != REFERENCE},
                            key=lambda i: next(lane.name for lane in self.lanes if lane.id == i).lower())
        self.slot = {lang_id: i for i, lang_id in enumerate(script_ids)}
        # the headline lanes: C++ first, then the script languages in alphabetical order
        self.headline = sorted((lane for lane in self.lanes if lane.headline),
                               key=lambda lane: (lane.id != REFERENCE, self.slot.get(lane.id, -1)))
        self.values = {}  # (runner, variant, benchmark, n) -> Value
        self.rows = {}  # group -> [(benchmark, n)]
        for run in data['runs']:
            if run['runner'] not in languages:
                continue  # only n/a processes, no language information
            self.values[(run['runner'], run['variant'], run['benchmark'], run['n'])] = Value(run)
            rows = self.rows.setdefault(run['group'], [])
            if (run['benchmark'], run['n']) not in rows:
                rows.append((run['benchmark'], run['n']))

    def value(self, lane, benchmark, n):
        return self.values.get((lane.runner, lane.variant, benchmark, n))

    def series_benchmarks(self):
        counts = {}
        for rows in self.rows.values():
            for benchmark, _ in rows:
                counts[benchmark] = counts.get(benchmark, 0) + 1
        return [b for b, c in counts.items() if c > 1]

    def group_of(self, benchmark):
        return next(g for g, rows in self.rows.items() if any(b == benchmark for b, _ in rows))


# === SVG ===

def esc(text):
    return str(text).replace('&', '&amp;').replace('<', '&lt;').replace('>', '&gt;')


class Svg:
    def __init__(self, width, height, theme):
        self.width, self.height, self.t = width, height, THEMES[theme]
        self.parts = []

    def text(self, x, y, s, size=12, color='primary', anchor='start', weight=None):
        w = f' font-weight="{weight}"' if weight else ''
        self.parts.append(f'<text x="{x:.1f}" y="{y:.1f}" font-size="{size}" fill="{self.t.get(color, color)}" '
                          f'text-anchor="{anchor}"{w}>{esc(s)}</text>')

    def line(self, x1, y1, x2, y2, color, width=1.0, opacity=None):
        o = f' stroke-opacity="{opacity}"' if opacity else ''
        self.parts.append(f'<line x1="{x1:.1f}" y1="{y1:.1f}" x2="{x2:.1f}" y2="{y2:.1f}" '
                          f'stroke="{self.t.get(color, color)}" stroke-width="{width}" stroke-linecap="round"{o}/>')

    def polyline(self, points, color, width=2.0):
        p = ' '.join(f'{x:.1f},{y:.1f}' for x, y in points)
        self.parts.append(f'<polyline points="{p}" fill="none" stroke="{color}" stroke-width="{width}" '
                          f'stroke-linejoin="round" stroke-linecap="round"/>')

    def dot(self, x, y, color, r=4.0):
        # a 2px ring in the surface color keeps overlapping marks legible
        self.parts.append(f'<circle cx="{x:.1f}" cy="{y:.1f}" r="{r}" fill="{color}" stroke="{self.t["surface"]}" '
                          f'stroke-width="2"/>')

    def render(self, title):
        return (f'<svg xmlns="http://www.w3.org/2000/svg" width="{self.width}" height="{self.height:.0f}" '
                f'viewBox="0 0 {self.width} {self.height:.0f}" font-family="{FONT}" role="img">\n'
                f'<title>{esc(title)}</title>\n'
                f'<rect x="0.5" y="0.5" width="{self.width - 1}" height="{self.height - 1:.0f}" rx="8" '
                f'fill="{self.t["surface"]}" stroke="{self.t["border"]}"/>\n'
                + '\n'.join(self.parts) + '\n</svg>\n')


def decades(values):
    lo = 10 ** math.floor(math.log10(min(values)))
    hi = 10 ** math.ceil(math.log10(max(values)))
    return (lo, hi * 10) if hi <= lo else (lo, hi)


def log_scale(lo, hi, x0, x1):
    a, b = math.log10(lo), math.log10(hi)
    return lambda v: x0 + (math.log10(v) - a) / (b - a) * (x1 - x0)


def decade_ticks(lo, hi):
    ticks, v = [], lo
    while v <= hi * 1.0001:
        ticks.append(v)
        v *= 10
    return ticks


def legend(svg, x, y, items, colors):
    """A row of legend items: (label, kind) with kind 'dot' or 'tick'."""
    for label, kind in items:
        if kind == 'tick':
            svg.line(x + 4, y - 9, x + 4, y + 3, colors[label], 2)
        else:
            svg.dot(x + 4, y - 3, colors[label])
        svg.text(x + 14, y + 1, label, 12, 'secondary')
        x += 14 + 7.0 * len(label) + 18
    return x


def group_chart(rs, group, title, theme):
    """A grouped dot plot: one band per benchmark and n, one sub-row per script language, C++ as a reference tick."""
    langs = [lane for lane in rs.headline if lane.id != REFERENCE]
    ref = next((lane for lane in rs.headline if lane.id == REFERENCE), None)
    rows = rs.rows[group]
    values = [v for lane in rs.headline for b, n in rows if (val := rs.value(lane, b, n)) and val.ok
              for v in val.medians]
    if not values:
        return None
    lo, hi = decades(values)

    width, left, right = 880, 210, 24
    sub, band_pad = 11, 6
    band = len(langs) * sub + 2 * band_pad
    top = 104
    height = top + len(rows) * band + 64
    svg = Svg(width, height, theme)
    colors = {lane.name: SLOTS[theme][rs.slot[lane.id]] for lane in langs}
    x = log_scale(lo, hi, left, width - right)
    if ref and not any((v := rs.value(ref, b, n)) and v.ok for b, n in rows):
        ref = None  # C++ embeds nothing: the host group

    svg.text(16, 28, title, 15, 'primary', weight='600')
    svg.text(16, 48, 'Median time per call, log scale. Dot: the median of the processes; line: their range. '
                     'Headline variants.', 12, 'secondary')
    items = [(lane.name, 'dot') for lane in langs]
    if ref:
        colors['C++ (reference)'] = THEMES[theme]['muted']
        items.append(('C++ (reference)', 'tick'))
    legend(svg, 16, 74, items, colors)

    ticks = decade_ticks(lo, hi)
    bottom = top + len(rows) * band
    for t in ticks:
        svg.line(x(t), top - 4, x(t), bottom + 4, 'grid')
        svg.text(x(t), top - 10, fmt_time(t), 11, 'muted', 'middle')
        svg.text(x(t), bottom + 18, fmt_time(t), 11, 'muted', 'middle')
    for i, (benchmark, n) in enumerate(rows):
        y0 = top + i * band
        if i > 0:
            svg.line(16, y0, width - right, y0, 'grid')
        svg.text(16, y0 + band / 2 + 4, benchmark, 12, 'primary')
        svg.text(left - 10, y0 + band / 2 + 4, f'n={fmt_n(n)}', 11, 'muted', 'end')
        if ref and (v := rs.value(ref, benchmark, n)) and v.ok:
            svg.line(x(v.median), y0 + 3, x(v.median), y0 + band - 3, 'muted', 2)
        for k, lane in enumerate(langs):
            y = y0 + band_pad + k * sub + sub / 2
            v = rs.value(lane, benchmark, n)
            color = SLOTS[theme][rs.slot[lane.id]]
            if v is None or not v.ok:
                svg.text(left + 4, y + 3.5, 'n/a' if v is None or v.status == 'n/a' else v.status, 9, 'muted')
                continue
            x1, x2 = x(min(v.medians)), x(max(v.medians))
            if x2 - x1 > 2:
                svg.line(x1, y, x2, y, color, 2, 0.6)
            svg.dot(x(v.median), y, color)
    svg.text(16, height - 14, setup_label(rs.data), 11, 'muted')
    return svg.render(f'{title}: median time per call')


def series_chart(rs, benchmark, theme):
    """A line chart over the series of n of one benchmark, log-log."""
    group = rs.group_of(benchmark)
    ns = [n for b, n in rs.rows[group] if b == benchmark]
    lanes = [lane for lane in rs.headline if any((v := rs.value(lane, benchmark, n)) and v.ok for n in ns)]
    values = [rs.value(lane, benchmark, n).median for lane in lanes for n in ns
              if (v := rs.value(lane, benchmark, n)) and v.ok]
    if not values:
        return None
    width, height = 560, 300
    left, right, top, bottom = 64, 150, 52, height - 56
    svg = Svg(width, height, theme)
    lo, hi = decades(values)
    y = log_scale(lo, hi, bottom, top)
    if max(ns) / min(ns) >= 10:
        x = log_scale(min(ns), max(ns), left + 12, width - right - 12)
    else:
        x = lambda n: left + 12 + (n - min(ns)) / (max(ns) - min(ns)) * (width - right - left - 24)

    svg.text(16, 26, f'{benchmark}: median time per call over n', 14, 'primary', weight='600')
    for t in decade_ticks(lo, hi):
        svg.line(left, y(t), width - right, y(t), 'grid')
        svg.text(left - 8, y(t) + 4, fmt_time(t), 11, 'muted', 'end')
    svg.line(left, bottom, width - right, bottom, 'axis')
    for n in ns:
        svg.text(x(n), bottom + 18, fmt_n(n), 11, 'muted', 'middle')

    ly = top + 4
    for lane in lanes:
        color = THEMES[theme]['muted'] if lane.id == REFERENCE else SLOTS[theme][rs.slot[lane.id]]
        points = [(x(n), y(v.median)) for n in ns if (v := rs.value(lane, benchmark, n)) and v.ok]
        if len(points) > 1:
            svg.polyline(points, color)
        for px, py in points:
            svg.dot(px, py, color)
        svg.line(width - right + 16, ly, width - right + 32, ly, color, 2)
        svg.dot(width - right + 24, ly, color)
        svg.text(width - right + 40, ly + 4, 'C++ (reference)' if lane.id == REFERENCE else lane.name, 12, 'secondary')
        ly += 20
    svg.text(16, height - 12, setup_label(rs.data), 10, 'muted')
    return svg.render(f'{benchmark}: median time per call over n')


# === Markdown ===

def picture(name, alt):
    return (f'<picture>\n  <source media="(prefers-color-scheme: dark)" srcset="charts/{name}-dark.svg">\n'
            f'  <img alt="{esc(alt)}" src="charts/{name}-light.svg">\n</picture>')


def cell(v, ref=None):
    if v is None or v.status == 'n/a':
        return 'n/a'
    if not v.ok:
        return f'**{v.status}**'
    notes = []
    if ref is not None and ref.ok:
        notes.append(fmt_factor(v.median / ref.median))
    if v.spread is not None and len(v.medians) > 1:
        notes.append(f'{v.spread * 100:.1f}%')
    if v.budget_limited:
        notes.append('budget')
    return f'**{fmt_time(v.median)}**' + (f'<br><sub>{" · ".join(notes)}</sub>' if notes else '')


def group_table(rs, group):
    lanes = rs.headline
    ref = next((lane for lane in lanes if lane.id == REFERENCE), None)
    if ref and all((v := rs.value(ref, b, n)) is None or not v.ok for b, n in rs.rows[group]):
        lanes = [lane for lane in lanes if lane is not ref]  # C++ embeds nothing: the host group
        ref = None
    lines = ['| Benchmark | n | ' + ' | '.join(lane.label for lane in lanes) + ' |',
             '|---|--:|' + '--:|' * len(lanes)]
    for benchmark, n in rs.rows[group]:
        ref_value = rs.value(ref, benchmark, n) if ref else None
        cells = [cell(rs.value(lane, benchmark, n), ref_value if lane is not ref else None) for lane in lanes]
        lines.append(f'| [{benchmark}](../../benchmarks/{benchmark}/README.md) | {fmt_n(n)} | ' + ' | '.join(cells) + ' |')
    return '\n'.join(lines)


def memory_table(rs, group):
    lanes = rs.headline
    ref = next((lane for lane in lanes if lane.id == REFERENCE), None)
    lines = ['| Benchmark | n | ' + ' | '.join(lane.label for lane in lanes) + ' |',
             '|---|--:|' + '--:|' * len(lanes)]
    for benchmark, n in rs.rows[group]:
        base = rs.value(ref, benchmark, n) if ref else None
        base = base.peak_any if base else None
        cells = []
        for lane in lanes:
            v = rs.value(lane, benchmark, n)
            if lane is ref:
                cells.append(f'{fmt_mib(base)}' if base is not None else 'n/a')
            elif v is None or v.peak is None:
                cells.append('n/a' if v is None or v.status == 'n/a' else f'**{v.status}**')
            else:
                cells.append(f'**{fmt_mib(v.peak)}**' + (f'<br><sub>{fmt_mib(v.peak - base, True)}</sub>' if base else ''))
        lines.append(f'| {benchmark} | {fmt_n(n)} | ' + ' | '.join(cells) + ' |')
    return '\n'.join(lines)


def variant_table(rs, lang_id):
    lanes = [lane for lane in rs.lanes if lane.id == lang_id]
    head = next((lane for lane in lanes if lane.headline), lanes[0])
    lanes.sort(key=lambda lane: (not lane.headline, lane.runner, lane.variant))
    lines = ['| Benchmark | n | ' + ' | '.join((lane.variant or '(no name)') + (' (headline)' if lane.headline else '')
                                                for lane in lanes) + ' |',
             '|---|--:|' + '--:|' * len(lanes)]
    for group, _ in GROUPS:
        for benchmark, n in rs.rows.get(group, []):
            values = [rs.value(lane, benchmark, n) for lane in lanes]
            if sum(1 for v in values if v is not None and v.status != 'n/a') < 2:
                continue  # nothing to compare
            ref = rs.value(head, benchmark, n)
            cells = [cell(v, ref if lane is not head else None) for lane, v in zip(lanes, values)]
            lines.append(f'| {benchmark} | {fmt_n(n)} | ' + ' | '.join(cells) + ' |')
    return '\n'.join(lines) if len(lines) > 2 else None


def load_summary(data):
    loads = [p['load']['other_cpus'] for r in data['runs'] for p in r['processes']
             if p.get('load') and p['wall_s'] >= 1.0]
    if not loads:
        return 'not measured'
    return (f'other processes used {statistics.median(loads):.2f} logical CPUs in the median, at most '
            f'{max(loads):.2f} (processes of at least 1 s)')


def setup_table(rs, status):
    d, s = rs.data, rs.system
    st = d['settings']
    runner = st['runner'] or {}
    pin = f'pinned to logical CPU {st["cpu"]}' if st['cpu'] is not None else 'not pinned'
    rows = [
        ('CPU', f'{s["cpu"]}, {s["cpu_cores"]} cores, {s["cpu_threads"]} threads'),
        ('Memory', fmt_bytes(s['memory_bytes'])),
        ('OS', f'{s.get("os_name") or s["os"]} ({s["os"]})'),
        ('Virtualization', virtualization(s)),
        ('Compiler', f'{s["compiler"]}, {s["build_config"]}, `{s["compiler_flags"]}`'),
        ('cpp-script-bench', d['tool'].replace('cpp-script-bench ', '')),
    ]
    langs = {}  # the version of the headline variant's runner (the runners of a language may differ in options)
    for lane in sorted(rs.lanes, key=lambda lane: not lane.headline):
        langs.setdefault(lane.id, lane)
    for lane in sorted(langs.values(), key=lambda lane: (lane.id != REFERENCE, rs.slot.get(lane.id, -1))):
        if lane.id != REFERENCE:
            rows.append((lane.name, lane.version))
    sessions = d.get('sessions', 1)
    rows += [
        ('Measured', f'{d["started"].replace("T", " ")[:16]} to {d["finished"].replace("T", " ")[:16]} UTC'
                     + (f', in {sessions} sessions (interrupted and resumed)' if sessions > 1 else '')),
        ('Processes', f'{st["processes"]} per run, interleaved across the languages; {pin}, priority {st["priority"]}'
                      + (f'; power: {d["power"]}' if d.get('power') else '')),
        ('Per process', f'{runner.get("warmup")} warm-up and {runner.get("runs")} measured calls, '
                        f'time budget {runner.get("budget_s")} s (at least {runner.get("min_runs")} calls)'),
        ('Machine load', load_summary(d)),
        ('Status', status),
    ]
    return '| | |\n|---|---|\n' + '\n'.join(f'| {k} | {v} |' for k, v in rows)


def write_set(rs, out, status, draft):
    charts = out / 'charts'
    if charts.exists():
        shutil.rmtree(charts)
    charts.mkdir(parents=True)

    md = [f'# Result set {out.name}', '']
    if draft:
        md += ['> **Draft:** this set is incomplete, has problems, or was built from a modified checkout. '
               'It is not a valid result.', '']
    if status == 'preview':
        md += ['> **Preview:** at least one language is at a pre-release version (TeaScript before 0.18.0). '
               'The set will be measured again for the release.', '']
    md += [setup_table(rs, status), '',
           'All numbers compare only within this set. **How to read:** each cell is the median time of one call of '
           '`bench( n )` (host API benchmarks: of one sample) over all processes: the median of the medians of the '
           'processes. Below it: the factor to C++, and the spread between the processes (slowest minus fastest '
           'process median, relative to the median); "budget": the time budget limited the number of calls. '
           'The headline variant of each language is shown; the other variants follow below. '
           'The rules are in [PLAN.md](../../PLAN.md#2-fairness-rules), the raw data in [results.json](results.json).',
           '']
    for group, title in GROUPS:
        if group not in rs.rows:
            continue
        md += [f'## {title}', '']
        for theme in THEMES:
            svg = group_chart(rs, group, title, theme)
            if svg:
                (charts / f'{group}-{theme}.svg').write_text(svg, encoding='utf-8')
        if (charts / f'{group}-light.svg').exists():
            md += [picture(group, f'{title}: median time per call of each language, log scale'), '']
        md += [group_table(rs, group), '']

    series = rs.series_benchmarks()
    if series:
        md += ['## Series', '', 'Benchmarks with a series of workload sizes, over n (log-log).', '']
        for benchmark in series:
            for theme in THEMES:
                svg = series_chart(rs, benchmark, theme)
                if svg:
                    (charts / f'series-{benchmark}-{theme}.svg').write_text(svg, encoding='utf-8')
            if (charts / f'series-{benchmark}-light.svg').exists():
                md += [picture(f'series-{benchmark}', f'{benchmark}: median time per call over n'), '']

    variant_tables = []
    for lang_id in sorted({lane.id for lane in rs.lanes}, key=lambda i: rs.slot.get(i, -1)):
        table = variant_table(rs, lang_id)
        if table:
            name = next(lane.name for lane in rs.lanes if lane.id == lang_id)
            variant_tables += [f'### {name}', '', table, '']
    if variant_tables:
        md += ['## Variants', '', 'All variants of each language; the factor is relative to its headline variant.',
               ''] + variant_tables

    if any(v.peak is not None for v in rs.values.values()):
        md += ['## Peak memory', '',
               'The peak memory of each process, the median over the processes of a run: the peak working set on '
               'Windows, the peak resident set on Linux. It covers the whole process: the runtime and libraries of the '
               'language, the unmeasured warm-up engine, the engine of the run and its workload. Below it: the '
               'difference to the process of the C++ runner for the same benchmark and n, which is the harness alone '
               'where C++ is "n/a" (the host API benchmarks). Headline variants.', '']
        for group, title in GROUPS:
            if group in rs.rows:
                md += [f'### {title}', '', memory_table(rs, group), '']

    if rs.data['problems']:
        md += ['## Problems', ''] + [f'- `{p}`' for p in rs.data['problems']] + ['']
    md += ['---', f'Generated by `runner/report.py` from [results.json](results.json).', '']
    (out / 'README.md').write_text('\n'.join(md), encoding='utf-8')


def write_index(results_dir):
    rows = []
    for f in sorted(results_dir.glob('*/results.json')):
        data = json.loads(f.read_text(encoding='utf-8'))
        meta = json.loads((f.parent / 'set.json').read_text(encoding='utf-8')) if (f.parent / 'set.json').exists() else {}
        s = data['system']
        rows.append(f'| [{f.parent.name}]({f.parent.name}/README.md) | {short_cpu(s["cpu"])} | '
                    f'{s.get("os_name") or s["os"]} | {virtualization(s)} | {s["compiler"]} | '
                    f'{(data["started"] or "")[:10]} | {data["tool"].replace("cpp-script-bench ", "")} | '
                    f'{meta.get("status", "")} |')
    md = ['# Result sets', '',
          'Each set is measured on one machine with one toolchain. Numbers compare only within a set: the sets '
          'differ in hardware, OS and compiler. How the sets are measured: [PLAN.md](../PLAN.md), section 3 and the '
          'decision log of 2026-10-29.', '',
          '| Set | CPU | OS | Virtualization | Compiler | Measured | cpp-script-bench | Status |',
          '|---|---|---|---|---|---|---|---|'] + rows + ['']
    (results_dir / 'README.md').write_text('\n'.join(md), encoding='utf-8')


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    parser.add_argument('results', type=Path, help='the results.json of a work directory of run.py')
    parser.add_argument('--name', help='the name of the set (default: from its compiler, CPU and OS)')
    parser.add_argument('--results-dir', type=Path, default=Path(__file__).resolve().parent.parent / 'results',
                        help='the directory of all sets (default: results/ of the repository)')
    parser.add_argument('--status', choices=['preview', 'regular'], default='preview',
                        help='preview (default): a language is at a pre-release version (TeaScript before 0.18.0)')
    parser.add_argument('--draft', action='store_true',
                        help='also publish an incomplete set, one with problems or from a modified checkout (for tests)')
    args = parser.parse_args()

    data = json.loads(args.results.read_text(encoding='utf-8'))
    reasons = []
    if not data.get('complete'):
        reasons.append('the set is incomplete')
    if data.get('problems'):
        reasons.append(f'{len(data["problems"])} problems')
    if data.get('dirty'):
        reasons.append('built from a modified checkout')
    if reasons and not args.draft:
        sys.exit(f'not published: {", ".join(reasons)} (--draft for a test)')

    rs = ResultSet(data)
    out = args.results_dir / (args.name or default_name(data['system']))
    out.mkdir(parents=True, exist_ok=True)
    if args.results.resolve() != (out / 'results.json').resolve():
        shutil.copyfile(args.results, out / 'results.json')
    status = args.status + (', draft' if reasons else '')
    (out / 'set.json').write_text(json.dumps({'status': status, 'published': datetime.datetime.now(
        datetime.timezone.utc).strftime('%Y-%m-%dT%H:%M:%SZ')}, indent=1) + '\n', encoding='utf-8')
    write_set(rs, out, args.status, bool(reasons))
    write_index(args.results_dir)
    print(f'{out / "README.md"} written ({len(rs.values)} runs)')
    return 0


if __name__ == '__main__':
    sys.exit(main())
