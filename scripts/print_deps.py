#!/usr/bin/env python3

"""
Prints the version of each dependency in the external directory, along with
its remote URL.

TODO: I think this will currently break if the commit is not tagged. We should
      probably handle that case and print the commit hash instead.
"""

import subprocess
from os import listdir
from pathlib import Path


def git(*args: str, cwd: Path) -> str:
    try:
        return (
            subprocess.run(
                ["git", *args],
                cwd=cwd,
                check=True,
                capture_output=True,
            )
            .stdout.decode("utf-8")
            .strip()
        )
    except:
        return "N/A"


def print_row(*args: tuple[str, int]) -> None:
    print(f"| {' | '.join(f'{arg[0]:<{arg[1]}}' for arg in args)} |")


def main() -> None:
    root_dir = Path(__file__).parent.parent / "external"

    deps: list[tuple[str, str, str, str]] = []

    for dependency in listdir(root_dir):
        dependency_root = root_dir / dependency

        version = git("describe", "--tags", "--exact-match", cwd=dependency_root)
        commit = git("rev-parse", "--short", "HEAD", cwd=dependency_root)
        remote = git("remote", "get-url", "origin", cwd=dependency_root)

        deps.append((dependency, version, commit, remote))

    dl = max(len(dep[0]) for dep in deps)
    vl = max(len(dep[1]) for dep in deps)
    cl = max(len(dep[2]) for dep in deps)
    rl = max(len(dep[3]) for dep in deps)

    print_row(("Dependency", dl), ("Version", vl), ("Commit", cl), ("Remote", rl))
    print_row(("-" * dl, dl), ("-" * vl, vl), ("-" * cl, cl), ("-" * rl, rl))

    for dep, version, commit, remote in deps:
        print_row((dep, dl), (version, vl), (commit, cl), (remote, rl))


if __name__ == "__main__":
    main()
