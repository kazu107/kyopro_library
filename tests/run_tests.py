# https://github.com/kazu107/kyopro_library/blob/main/tests/run_tests.py
# SPDX-License-Identifier: CC0-1.0
"""GNU C++17 regression tests, standalone headers, README examples and repository metadata."""
from __future__ import annotations

import argparse
from pathlib import Path
import re
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
EXPECTED_EXAMPLES = {
    "README.md": ["2 3\n3 2\n5 1\n"],
    "data_structure/README.md": [
        "4\n1 2\n24 4 8 4\n1 1\n3 0\n3\n",
        "2\n3\n0\n",
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


def check_metadata() -> tuple[list[Path], list[Path]]:
    sources = source_files()
    libraries = [path for path in sources if "tests" not in path.relative_to(ROOT).parts]
    headers = [path for path in libraries if path.suffix == ".hpp"]
    for source in sources:
        relative = source.relative_to(ROOT).as_posix()
        prefix = "#" if source.suffix == ".py" else "//"
        lines = source.read_text(encoding="utf-8").splitlines()
        assert lines[0] == f"{prefix} https://github.com/kazu107/kyopro_library/blob/main/{relative}", relative
        assert lines[1] == f"{prefix} SPDX-License-Identifier: CC0-1.0", relative
    for markdown in ROOT.rglob("*.md"):
        if any(part in {".git", "build"} for part in markdown.relative_to(ROOT).parts):
            continue
        contents = markdown.read_text(encoding="utf-8")
        fence = chr(96) * 3
        prose = re.sub(fence + r".*?" + fence, "", contents, flags=re.S)
        for target in re.findall(r"\]\(([^)]+)\)", prose):
            if target.startswith(("https://", "http://", "#")):
                continue
            target = target.split("#", 1)[0]
            assert (markdown.parent / target).exists(), (markdown, target)
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
    print(f"PASS metadata: {len(libraries)} library files, {len(sources)} source URL/SPDX headers, links/counts/CC0", flush=True)
    return libraries, headers


def run_command(command: list[str], *, code: str | None = None) -> str:
    result = subprocess.run(
        command, input=code, cwd=ROOT, text=True, encoding="utf-8", errors="replace",
        capture_output=True, timeout=120,
    )
    if result.returncode:
        raise RuntimeError(
            f"Command failed ({result.returncode}): {' '.join(command)}\n{result.stdout}{result.stderr}"
        )
    return result.stdout


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cxx", default="g++", help="C++ compiler executable")
    parser.add_argument("--debug", action="store_true", help="Enable checked libstdc++ iterators and debug build")
    args = parser.parse_args()
    compiler = shutil.which(args.cxx)
    if compiler is None:
        raise RuntimeError(f"C++ compiler not found: {args.cxx}")
    _, headers = check_metadata()
    build = ROOT / "build" / ("debug" if args.debug else "release")
    build.mkdir(parents=True, exist_ok=True)
    flags = ["-std=c++17", "-Wall", "-Wextra", "-Werror", "-I", "."]
    flags += ["-O0", "-g", "-D_GLIBCXX_DEBUG", "-D_GLIBCXX_ASSERTIONS"] if args.debug else ["-O2"]
    extension = ".exe" if sys.platform == "win32" else ""
    for header in headers:
        relative = header.relative_to(ROOT).as_posix()
        run_command([compiler, *flags, "-fsyntax-only", "-x", "c++", "-"], code=f'#include "{relative}"\n')
    combined = "\n".join(f'#include "{p.relative_to(ROOT).as_posix()}"' for p in headers)
    combined += '\n#include "tree/Subtreesize.cpp"\n#include "tree/rerooting.cpp"\n#include "data_structure/LagySegTree.cpp"\n'
    run_command([compiler, *flags, "-fsyntax-only", "-x", "c++", "-"], code=combined)
    print(f"PASS headers: {len(headers)} standalone includes and combined include", flush=True)
    for source in sorted((ROOT / "tests").glob("*_test.cpp")):
        executable = build / (source.stem + extension)
        print(f"RUN {source.name}", flush=True)
        run_command([compiler, *flags, source.relative_to(ROOT).as_posix(), "-o", str(executable)])
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
            executable = build / (f"readme_{example_count}" + extension)
            run_command([compiler, *flags, "-x", "c++", "-", "-o", str(executable)], code=code)
            output = run_command([str(executable)])
            assert output == expected, (relative, repr(output), repr(expected))
            example_count += 1
    print(f"PASS README: {example_count} programs compiled and matched expected output", flush=True)
    print("ALL TESTS PASSED" + (" (debug)" if args.debug else " (release)"), flush=True)


if __name__ == "__main__":
    try:
        main()
    except (AssertionError, RuntimeError, subprocess.TimeoutExpired) as error:
        print(f"FAIL: {error}", file=sys.stderr)
        sys.exit(1)
