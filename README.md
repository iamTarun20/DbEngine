# DbEngine

A small C++17 learning project for page storage, a buffer pool, and a limited SQL-like command line. It is a prototype, not a production database.

## Build and run

Requires CMake 3.16+ and a C++17 compiler.

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/DbEngine
```

On Windows, the executable may be under `build/Debug/DbEngine.exe`. The database file `dbengine.db` is created in the current working directory. Exit normally, or send EOF, to flush dirty pages.

```sql
INSERT INTO students VALUES 'Tarun Kumar';
INSERT INTO students VALUES Alice;
SELECT * FROM students;
exit
```

Keywords and table names are case-insensitive. Values can be one unquoted word or one single-quoted string; quote escaping, column definitions, and other SQL are not supported. Tables are logical names, created on first insert. All rows share one 4 KiB page; when it fills, inserts are rejected without altering existing records. There is no DELETE, UPDATE, schema, multi-page allocation, or query optimizer.

## Design and limitations

The parser validates the supported syntax. The execution engine prefixes rows with their table name, stores them in a slotted page, and filters that page for SELECT. The buffer pool has three frames, pins pages while in use, evicts the least recently used unpinned frame, and writes dirty pages on eviction or explicit flush. The CLI flushes on normal shutdown. File writes are not crash-safe and there is no WAL, transaction recovery, concurrency control in the execution path, or guarantee against data loss on an abnormal exit.

The B+ tree leaf node and lock manager are isolated experiments: they are not used by queries and do not constitute a full index or ACID transactions. The node's `-1` lookup result is reserved for missing keys. Page fields use the machine's native byte order; database files are not portable across architectures.

**Old prototype files:** previous versions did not store table names with rows and did not reliably flush. This version cannot determine the original table for such rows, so they are not returned by queries. Keep a backup of an old `dbengine.db`; start with a new file rather than relying on automatic migration.
