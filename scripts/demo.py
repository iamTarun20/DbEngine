"""Run reproducible persistence and cache workloads against the DbEngine CLI."""

import argparse
from pathlib import Path
import subprocess
from tempfile import TemporaryDirectory


PAGE_BYTES = 4096
FRAMES = 3
INSERTED = "1 row inserted."
OVERSIZED = "Row exceeds the capacity of one 4 KiB page."


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def run_cli(executable, database, commands):
    result = subprocess.run(
        [str(executable), "--batch", str(database)],
        input="\n".join(commands) + "\n",
        capture_output=True,
        text=True,
        timeout=60,
    )
    require(result.returncode == 0, f"DbEngine exited with {result.returncode}: {result.stderr}")
    return result.stdout.splitlines()


def event_rows(count):
    return [f"event-{number:05d}-{'x' * 48}" for number in range(count)]


def insert_commands(rows):
    return [f"INSERT INTO events VALUES '{row}';" for row in rows]


def read_stats(line):
    return {key: int(value) for key, value in (field.split("=") for field in line.split())}


def persistence_demo(executable, database):
    rows = event_rows(10_000)
    commands = insert_commands(rows)
    commands.insert(len(commands) // 2, "INSERT INTO notes VALUES 'keep tables separate';")
    require(run_cli(executable, database, commands) == [INSERTED] * len(commands),
            "bulk inserts did not all succeed")

    # Each CLI invocation opens the file in a new process and starts with a cold cache.
    result = run_cli(executable, database, ["SELECT * FROM events;", ".stats",
                                            "SELECT * FROM notes;"])
    require(result[:-4] == rows, "bulk scan changed the rows or their insertion order")
    require(result[-4] == f"{len(rows)} row(s) returned.", "bulk scan row count mismatch")
    require(result[-2:] == ["keep tables separate", "1 row(s) returned."], "table isolation failed")
    stats = read_stats(result[-3])
    pages = stats["pages"]
    require(pages > FRAMES and database.stat().st_size == pages * PAGE_BYTES,
            "bulk load did not create a contiguous file larger than the buffer")
    require(stats["page_reads"] == pages and stats["evictions"] == pages - FRAMES,
            "cold scan I/O did not match the page count")
    print(f"Persistence: {len(rows):,} event rows verified after restart across {pages} pages "
          f"({PAGE_BYTES:,} bytes each, {FRAMES} buffer frames).")
    print(f"Cold scan: {stats['page_reads']} page reads and {stats['evictions']} evictions.")


def cache_demo(executable, database):
    rows = event_rows(150)
    scans = 10
    require(run_cli(executable, database, insert_commands(rows)) == [INSERTED] * len(rows),
            "cache workload setup failed")
    result = run_cli(executable, database, ["SELECT * FROM events;"] * scans + [".stats"])
    require(result[:-1] == (rows + [f"{len(rows)} row(s) returned."]) * scans,
            "repeated scans changed the query results")
    stats = read_stats(result[-1])
    require(stats["pages"] == FRAMES and stats["page_reads"] == FRAMES
            and stats["cache_misses"] == FRAMES and stats["evictions"] == 0
            and stats["cache_hits"] == (scans - 1) * FRAMES, "unexpected cache behavior")
    requests = stats["cache_hits"] + stats["cache_misses"]
    print(f"Cache: {stats['cache_hits']}/{requests} buffer hits and {stats['page_reads']} "
          f"page reads over {scans} scans of a {FRAMES}-page dataset.")


def oversized_row_demo(executable, database):
    oversized = [f"INSERT INTO events VALUES '{'x' * length}';"
                 for length in (PAGE_BYTES, 100_000)]
    result = run_cli(executable, database, oversized + ["SELECT * FROM events;"])
    require(result == [OVERSIZED, OVERSIZED, "0 row(s) returned."],
            "oversized insert was accepted")
    require(database.stat().st_size == 0, "oversized insert allocated a page")
    result = run_cli(executable, database, ["INSERT INTO events VALUES saved;"] + oversized
                     + ["SELECT * FROM events;"])
    require(result == [INSERTED, OVERSIZED, OVERSIZED, "saved", "1 row(s) returned."],
            "oversized insert changed an existing row")
    require(database.stat().st_size == PAGE_BYTES, "oversized insert grew an existing file")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("executable", type=Path, help="path to the built DbEngine executable")
    args = parser.parse_args()
    executable = args.executable.resolve(strict=True)
    with TemporaryDirectory(prefix="dbengine-demo-") as directory:
        root = Path(directory)
        persistence_demo(executable, root / "bulk.db")
        cache_demo(executable, root / "cache.db")
        oversized_row_demo(executable, root / "oversized.db")


if __name__ == "__main__":
    main()
