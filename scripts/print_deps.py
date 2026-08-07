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


def main() -> None:
    root_dir = Path(__file__).parent.parent / "external"

    deps: list[tuple[str, str, str]] = []

    for dependency in listdir(root_dir):
        dependency_root = root_dir / dependency

        tag = git("describe", "--tags", "--exact-match", cwd=dependency_root)
        remote = git("remote", "get-url", "origin", cwd=dependency_root)

        deps.append((dependency, tag, remote))

    dep_length = max(len(dep[0]) for dep in deps)
    tag_length = max(len(dep[1]) for dep in deps)
    rem_length = max(len(dep[2]) for dep in deps)

    print(
        f"| {'Dependency':<{dep_length}} | {'Tag':<{tag_length}} | {'Remote':<{rem_length}} |"
    )
    print(f"| {'-' * dep_length} | {'-' * tag_length} | {'-' * rem_length} |")

    for dep, tag, remote in deps:
        print(f"| {dep:<{dep_length}} | {tag:<{tag_length}} | {remote:<{rem_length}} |")


if __name__ == "__main__":
    main()
