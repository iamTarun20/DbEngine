# DbEngine

DbEngine is a C++17 database engine for a small SQL subset. It stores variable-length rows in a disk-backed file of 4 KiB slotted pages and caches up to three pages with least-recently-used (LRU) eviction.

## Quickstart

Build with a C++17 compiler and CMake 3.16 or later.

```sh
git clone https://github.com/iamTarun20/DbEngine.git
cd DbEngine
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/DbEngine example.db
```

On Windows with Visual Studio, run `.\build\Release\DbEngine.exe example.db` from PowerShell. The database path is optional and defaults to `dbengine.db` in the current directory. Use `--batch` before the path to read commands from stdin without prompts.

```sql
INSERT INTO students VALUES 'Tarun Kumar';
INSERT INTO students VALUES Alice;
SELECT * FROM students;
.stats
exit
```

Run the same executable again with the same path and select from `students` to read the saved rows. Tables are created implicitly on first insert. Table names and keywords are case-insensitive. Each row is one unquoted word or one single-quoted string, with semicolons optional.

## Reproduce the results

The demo uses Python 3's standard library and creates temporary databases. It exercises the built CLI, checks every returned row, and prints the observed counters.

```sh
python3 scripts/demo.py ./build/DbEngine
```

On Windows, pass the path to `DbEngine.exe` and use `python` if that is your Python launcher.

| Workload | Observed result |
| --- | --- |
| Load 10,000 event rows, exit, then reopen and scan | All 10,000 rows matched in order across 176 pages |
| Scan the 176-page file with an empty buffer | 176 page reads and 173 evictions |
| Scan a three-page dataset 10 times from an empty buffer | 27 hits in 30 requests and 3 page reads |

`.stats` reports counters for the current process. Page reads and writes count transfers through the disk manager. Dirty pages still in the buffer are flushed on clean exit or EOF.

To run the test suite, switch into the build directory and invoke CTest. CTest includes the CLI demo when Python 3 is available.

```sh
cd build
ctest -C Release --output-on-failure
```

## How a query runs

| Layer | Responsibility |
| --- | --- |
| CLI and parser | Read one statement per line and validate the supported `INSERT` and `SELECT` forms |
| Execution engine | Insert into the last page or append a page when full. Scan pages in file order and return rows for the requested table |
| Slotted page | Keep a directory of offsets and lengths for variable-length rows. Each row includes its table name |
| Buffer pool | Pin pages while queries use them, evict the least recently used unpinned frame, and write back dirty pages |
| Disk manager | Read or write fixed-size pages by file offset and recover the page count from file length on open |

The engine handles one process per database file. Each row must fit inside one page. Queries use a sequential scan of the shared row file. Schema definitions, indexes in the query path, transactions, and crash recovery are outside the supported subset. The B+ tree leaf and lock manager are standalone experiments.

Files from the previous table-aware, single-page version open directly because the slotted-page layout is unchanged. Earlier prototype rows lack table names and cannot be assigned to a table automatically. Page fields use native byte order, so keep database files on machines with the same byte order.
