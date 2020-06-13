//
// Created by potato on 07/06/2020.
//

#include <cassert>
#include <cstring>
#include <iostream>
#include <fstream>
#include "src/Table.h"
#include "src/Repl.h"

void test_insert() {
    Table *table = new Table;
    std::vector<std::string> instructions;
    instructions.emplace_back("insert 1 test test@email.com");
    instructions.emplace_back("insert 2 test2 test2@email.com");
    for(auto inst : instructions) {
        Statement stmt;
        Repl::stmtFromString(inst, stmt);
        table->insert(stmt.tokens);
    }
    Row* r = new Row;
    std::memcpy(r, table->rowSlot(0), sizeof(Row));
    assert(r->id == 1);
    assert((Table::compareStringToChar(r->username, "test")));
    assert((Table::compareStringToChar(r->email, "test@email.com")));

    std::memcpy(r, table->rowSlot(1), sizeof(Row));
    assert(r->id == 2);
    assert(Table::compareStringToChar(r->username, "test2"));
    assert(Table::compareStringToChar(r->email, "test2@email.com"));

    delete r;
    delete table;
}

void test_persistence(){
    // Clear contents
    std::ofstream f("./test_db", std::ofstream::trunc);
    f.close();
    auto *table = new Table("test_db");
    std::vector<std::string> instructions;
    instructions.emplace_back("insert 1 test test@email.com");
    instructions.emplace_back("insert 2 test2 test2@email.com");
    for(auto inst : instructions) {
        Statement stmt;
        Repl::stmtFromString(inst, stmt);
        table->insert(stmt.tokens);
    }
    table->tableClose();
    delete table;
    auto table2 = new Table("test_db");

    Row* r = new Row;
    std::memcpy(r, table2->rowSlot(0), sizeof(Row));
    assert(r->id == 1);
    assert((Table::compareStringToChar(r->username, "test")));
    assert((Table::compareStringToChar(r->email, "test@email.com")));

    std::memcpy(r, table2->rowSlot(1), sizeof(Row));
    assert(r->id == 2);
    assert(Table::compareStringToChar(r->username, "test2"));
    assert(Table::compareStringToChar(r->email, "test2@email.com"));

    delete r;
    delete table2;
}

int main() {
    test_insert();
    test_persistence();
    std::cout << "All tests passed\n";
    return 0;
}
