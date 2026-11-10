#include "buffer/buffer_pool_manager.h"
#include "parser/sql_parser.h"
#include "index/b_plus_tree_node.h"
#include "concurrency/lock_manager.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
using namespace dbengine;
void check(bool condition, const char* message) { if (!condition) throw std::runtime_error(message); }
int main() {
    const std::string path = "dbengine_test_temp.db";
    std::remove(path.c_str());
    try {
        SlottedPage page;
        check(page.IsValid(), "new page invalid");
        check(!page.InsertRecord(std::string(65540, 'x')), "oversized record accepted");
        check(page.InsertRecord("hello world"), "insert failed");
        check(page.GetRecord(0) == "hello world", "record mismatch");
        SQLParser parser;
        auto insert = parser.ParseQuery("InSeRt into Users values 'Tarun Kumar';");
        check(insert.type == StatementType::INSERT && insert.table_name == "users" && insert.record_data == "Tarun Kumar", "parser insert");
        check(parser.ParseQuery("SELECT * FROM users;").table_name == "users", "parser select");
        check(parser.ParseQuery("SELECT * FROM users WHERE x=1").type == StatementType::INVALID, "invalid query accepted");
        check(parser.ParseQuery(" \tINSERT into Users values 'a; b' ;\t").record_data == "a; b",
              "quoted semicolon was parsed as a terminator");
        check(parser.ParseQuery("INSERT INTO users VALUES 'two''quotes'").type == StatementType::INVALID,
              "unsupported quote escaping accepted");
        check(parser.ParseQuery("SELECT * FROM 2users").type == StatementType::INVALID,
              "invalid table name accepted");
        {
            DiskManager disk(path); BufferPoolManager pool(&disk);
            Page* first = pool.FetchPage(0);
            check(first && first->data_.InsertRecord("persisted"), "page insert failed");
            check(pool.UnpinPage(0, true), "unpin failed");
            check(!pool.UnpinPage(0, false), "double unpin accepted");
            Page* a = pool.FetchPage(1); Page* b = pool.FetchPage(2); Page* c = pool.FetchPage(3);
            check(a && b && c, "eviction failed");
            check(pool.FetchPage(4) == nullptr, "evicted pinned page");
            pool.UnpinPage(1, false); pool.UnpinPage(2, false); pool.UnpinPage(3, false);
            pool.FlushAllPages();
        }
        {
            DiskManager disk(path); BufferPoolManager pool(&disk);
            Page* restored = pool.FetchPage(0);
            check(restored && restored->data_.GetRecord(0) == "persisted", "persistence failed");
            pool.UnpinPage(0, false);
            check(pool.GetPageCount() == 1, "wrong page count for an existing file");

            SlottedPage next;
            check(next.InsertRecord("new page"), "page insert failed");
            Page* reserved = pool.FetchPage(1);
            check(reserved && !pool.AppendPage(next), "appended over a pinned page");
            check(pool.GetPageCount() == 1, "failed append changed page count");
            pool.UnpinPage(1, false);
            check(pool.AppendPage(next), "append failed");
            check(pool.GetPageCount() == 2, "append did not advance page count");
            SlottedPage persisted;
            check(disk.ReadPage(1, persisted.GetRawData()) && persisted.GetRecord(0) == "new page",
                  "appended page did not persist");
        }
        BPlusTreeNode node(true, 4);
        check(node.Insert(4, 40) && node.Insert(2, 20) && node.Insert(8, 80), "node insert");
        check(!node.Insert(2, 99), "duplicate key accepted");
        int promoted; std::unique_ptr<BPlusTreeNode> sibling(node.Split(promoted));
        check(promoted == 4 && node.FindValue(2) == 20 && sibling->FindValue(8) == 80, "node split");
        LockManager locks; Transaction first(1), second(2);
        check(locks.LockExclusive(&first, 42) && !locks.LockExclusive(&second, 42), "lock conflict");
        locks.Unlock(&first, 42);
        check(!locks.LockExclusive(&first, 7) && locks.LockExclusive(&second, 42), "lock state");
        std::remove(path.c_str());
        { std::ofstream truncated(path, std::ios::binary); truncated << "bad"; }
        bool caught = false;
        try { DiskManager disk(path); BufferPoolManager pool(&disk); pool.FetchPage(0); }
        catch (const std::runtime_error&) { caught = true; }
        check(caught, "truncated page accepted");
        std::remove(path.c_str());
        std::cout << "All tests passed.\n";
    } catch (const std::exception& error) {
        std::remove(path.c_str()); std::cerr << error.what() << '\n'; return 1;
    }
}
