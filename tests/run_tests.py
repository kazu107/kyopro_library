# https://github.com/kazu107/kyopro_library/blob/main/tests/run_tests.py
# SPDX-License-Identifier: CC0-1.0
# できること: 依存コメントからC++コードを並べ、比較テスト・サンプル・書式を検証する。
# 使い方: python tests/run_tests.py [--debug] [--cxx g++]
# 使い方: python tests/run_tests.py --bundle graph/dijkstra.hpp --output build/dijkstra_snippet.cpp
# 前提: Python 3.10以降。検証はGNU C++17対応コンパイラが必要。束ねるだけなら不要。
# 計算量: 依存展開はソース総量に比例。検証時間はコンパイルと各テストの計算量に依存。
"""Check include-free snippets, or bundle selected snippets in dependency order."""
from __future__ import annotations

import argparse
from pathlib import Path
import re
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
PRELUDE = chr(35) + "include <bits/stdc++.h>\nusing namespace std;\nusing ll = long long;\n"
INCLUDE = re.compile(r"^[ \t]*#[ \t]*include\b", re.M)
EXPECTED_EXAMPLES = {
    "README.md": ["2 3\n3 2\n5 1\n"],
    "data_structure/README.md": [
        "4\n1 2\n24 4 8 4\n1 1\n3 0\n3\n", "2\n3\n0\n",
    ],
    "graph/README.md": ["6\n0 1 2 3\n3\n"],
    "tree/README.md": ["14\n54\n1 3\n", "3 2 3\n"],
    "flow/README.md": ["3\n3 7\n2\n2\n"],
}


def source_files() -> list[Path]:
    return sorted(
        path for path in ROOT.rglob("*")
        if path.is_file()
        and not any(part in {".git", "build"} for part in path.relative_to(ROOT).parts)
        and path.suffix in {".cpp", ".hpp", ".py"}
    )


def source_path(relative: str | Path) -> Path:
    path = (ROOT / relative).resolve()
    if not path.is_relative_to(ROOT) or any(
        part in {".git", "build"} for part in path.relative_to(ROOT).parts
    ):
        raise ValueError(f"Snippet must be a repository source: {relative}")
    if path.suffix not in {".cpp", ".hpp"} or not path.is_file():
        raise ValueError(f"C++ snippet not found: {relative}")
    return path


def dependencies(path: Path) -> list[Path]:
    text = path.read_text(encoding="utf-8")
    markers = re.findall(r"^// 依存: (.+)$", text, flags=re.M)
    if len(markers) != 1:
        raise ValueError(f"Expected exactly one dependency comment: {path}")
    if markers[0] == "なし":
        return []
    return [source_path(item.strip()) for item in markers[0].split(",")]


def bundle_sources(entries: list[Path], *, line_directives: bool = False) -> str:
    """Emit each dependency once before its caller; never emit include directives."""
    seen: set[Path] = set()
    active: set[Path] = set()
    chunks: list[str] = []

    def visit(path: Path) -> None:
        path = source_path(path)
        if path in active:
            raise ValueError(f"Cyclic snippet dependency: {path.relative_to(ROOT)}")
        if path in seen:
            return
        active.add(path)
        for dependency in dependencies(path):
            visit(dependency)
        active.remove(path)
        seen.add(path)
        text = path.read_text(encoding="utf-8")
        if INCLUDE.search(text):
            raise ValueError(f"Unexpected include directive: {path.relative_to(ROOT)}")
        if line_directives:
            chunks.append(f'#line 1 "{path.relative_to(ROOT).as_posix()}"\n')
        chunks.append(text.rstrip() + "\n")

    for entry in entries:
        visit(entry)
    return "\n".join(chunks)


def check_metadata() -> list[Path]:
    sources = source_files()
    libraries = [path for path in sources if "tests" not in path.relative_to(ROOT).parts]
    for source in sources:
        relative = source.relative_to(ROOT).as_posix()
        prefix = "#" if source.suffix == ".py" else "//"
        text = source.read_text(encoding="utf-8")
        lines = text.splitlines()
        assert lines[0] == f"{prefix} https://github.com/kazu107/kyopro_library/blob/main/{relative}", relative
        assert lines[1] == f"{prefix} SPDX-License-Identifier: CC0-1.0", relative
        for heading in ("できること:", "使い方:", "前提:", "計算量:"):
            assert heading in text, (relative, heading)
        if source.suffix in {".cpp", ".hpp"}:
            assert not INCLUDE.search(text), relative
            assert not re.search(r"^[ \t]*#[ \t]*pragma[ \t]+once\b", text, re.M), relative
            dependencies(source)
    for markdown in ROOT.rglob("*.md"):
        if any(part in {".git", "build"} for part in markdown.relative_to(ROOT).parts):
            continue
        contents = markdown.read_text(encoding="utf-8")
        assert not INCLUDE.search(contents), markdown
        fence = chr(96) * 3
        prose = re.sub(fence + r".*?" + fence, "", contents, flags=re.S)
        for target in re.findall(r"\]\(([^)]+)\)", prose):
            if not target.startswith(("https://", "http://", "#")):
                assert (markdown.parent / target.split("#", 1)[0]).exists(), (markdown, target)
    readme = (ROOT / "README.md").read_text(encoding="utf-8")
    categories = re.findall(r"^\| \[([^\]]+)\]\(([^)]+)\) \| .* \| (\d+) \|$", readme, flags=re.M)
    assert categories
    for _, folder, count in categories:
        directory = ROOT / folder
        actual = len(list(directory.glob("*.cpp"))) + len(list(directory.glob("*.hpp")))
        assert actual == int(count), (folder, actual, count)
    assert sum(int(count) for _, _, count in categories) == len(libraries)
    for source in libraries:
        assert f"]({source.relative_to(ROOT).as_posix()})" in readme, source
    assert "CC0 1.0 Universal" in (ROOT / "LICENSE").read_text(encoding="utf-8")
    # 依存を共有する複数入口・重複入口を与えても、共有本体は1回だけ並ぶ。
    sample = bundle_sources([ROOT / "graph/dijkstra.hpp", ROOT / "graph/bfs.hpp", ROOT / "graph/dijkstra.hpp"])
    graph_url = "// https://github.com/kazu107/kyopro_library/blob/main/graph/Graph.hpp"
    result_url = "// https://github.com/kazu107/kyopro_library/blob/main/graph/ShortestPath.hpp"
    entry_url = "// https://github.com/kazu107/kyopro_library/blob/main/graph/dijkstra.hpp"
    assert sample.count(graph_url) == sample.count(result_url) == sample.count(entry_url) == 1
    assert sample.index(graph_url) < sample.index(result_url) < sample.index(entry_url)
    assert not INCLUDE.search(sample)
    print(f"PASS metadata: {len(libraries)} snippets, {len(sources)} URL/SPDX/comment headers, no includes, dependencies/links/counts", flush=True)
    return libraries


def run_command(command: list[str], *, code: str | None = None) -> str:
    result = subprocess.run(
        command, input=code, cwd=ROOT, text=True, encoding="utf-8", errors="replace",
        capture_output=True, timeout=120,
    )
    if result.returncode:
        raise RuntimeError(f"Command failed ({result.returncode}): {' '.join(command)}\n{result.stdout}{result.stderr}")
    return result.stdout


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cxx", default="g++", help="C++ compiler executable")
    parser.add_argument("--debug", action="store_true", help="Enable checked libstdc++ iterators")
    parser.add_argument("--bundle", nargs="+", metavar="FILE", help="Emit selected snippets with their dependencies")
    parser.add_argument("--output", type=Path, help="Bundle output file; default is stdout")
    parser.add_argument("--overwrite", action="store_true", help="Allow replacing an existing bundle output")
    args = parser.parse_args()
    if args.bundle:
        code = bundle_sources([source_path(item) for item in args.bundle])
        if args.output is None:
            sys.stdout.write(code)
        else:
            output = (ROOT / args.output).resolve()
            if output.exists() and not args.overwrite:
                raise FileExistsError(f"Output already exists; pass --overwrite: {output}")
            output.parent.mkdir(parents=True, exist_ok=True)
            output.write_text(code, encoding="utf-8")
            print(f"Wrote include-free bundle: {output}")
        return
    if args.output or args.overwrite:
        parser.error("--output/--overwrite require --bundle")
    compiler = shutil.which(args.cxx)
    if compiler is None:
        raise RuntimeError(f"C++ compiler not found: {args.cxx}")
    libraries = check_metadata()
    build = ROOT / "build" / ("debug" if args.debug else "release")
    build.mkdir(parents=True, exist_ok=True)
    flags = ["-std=c++17", "-Wall", "-Wextra", "-Werror"]
    flags += ["-O0", "-g", "-D_GLIBCXX_DEBUG", "-D_GLIBCXX_ASSERTIONS"] if args.debug else ["-O2"]
    extension = ".exe" if sys.platform == "win32" else ""
    legacy_warning = "math/modular/largeInt_mod.cpp"
    for source in libraries:
        relative = source.relative_to(ROOT).as_posix()
        # 既存コードの符号比較警告は、処理を変えずに維持する。
        allowed = ["-Wno-error=sign-compare"] if relative == legacy_warning else []
        run_command([compiler, *flags, *allowed, "-fsyntax-only", "-x", "c++", "-"],
                    code=PRELUDE + bundle_sources([source], line_directives=True))
    run_command([compiler, *flags, "-Wno-error=sign-compare", "-fsyntax-only", "-x", "c++", "-"],
                code=PRELUDE + bundle_sources(libraries, line_directives=True))
    print(f"PASS snippets: {len(libraries)} individual dependency bundles and combined bundle", flush=True)
    for source in sorted((ROOT / "tests").glob("*_test.cpp")):
        executable = build / (source.stem + extension)
        print(f"RUN {source.name}", flush=True)
        run_command([compiler, *flags, "-x", "c++", "-", "-o", str(executable)],
                    code=PRELUDE + bundle_sources([source], line_directives=True))
        print(run_command([str(executable)]).strip(), flush=True)
    example_count = 0
    fence = chr(96) * 3
    for relative, expected_outputs in EXPECTED_EXAMPLES.items():
        markdown = (ROOT / relative).read_text(encoding="utf-8")
        programs = [
            code for code in re.findall(fence + r"cpp\n(.*?)" + fence, markdown, flags=re.S)
            if "int main(" in code
        ]
        assert len(programs) == len(expected_outputs), relative
        for code, expected in zip(programs, expected_outputs):
            markers = re.findall(r"^// 使用するコード: (.+)$", code, flags=re.M)
            assert len(markers) == 1, relative
            entries = [source_path(item.strip()) for item in markers[0].split(",")]
            executable = build / (f"readme_{example_count}" + extension)
            run_command([compiler, *flags, "-x", "c++", "-", "-o", str(executable)],
                        code=PRELUDE + bundle_sources(entries, line_directives=True) + code)
            output = run_command([str(executable)])
            assert output == expected, (relative, repr(output), repr(expected))
            example_count += 1
    print(f"PASS README: {example_count} dependency-bundled examples matched expected output", flush=True)
    print("ALL TESTS PASSED" + (" (debug)" if args.debug else " (release)"), flush=True)


if __name__ == "__main__":
    try:
        main()
    except (AssertionError, RuntimeError, ValueError, OSError, subprocess.TimeoutExpired) as error:
        print(f"FAIL: {error}", file=sys.stderr)
        sys.exit(1)
