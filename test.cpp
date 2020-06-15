//
// Created by potato on 07/06/2020.
//

#include <cassert>
#include <cstring>
#include <iostream>
#include <fstream>
#include "src/Table.h"
#include "src/Repl.h"
bool compareStringToChar(const char *c, const std::string& s) {
    for (auto i = 0; i < s.length(); i++) {
        if (c[i] == '\0' || c[i] != s[i])
            return false;
    }
    return true;
}

void test_insert() {
    auto *table = new Table();
    std::vector<std::string> instructions;
    instructions.emplace_back("insert 1 test test@email.com");
    instructions.emplace_back("insert 2 test2 test2@email.com");
    for(auto inst : instructions) {
        Statement stmt;
        Repl::stmtFromString(inst, stmt);
        table->insert(stmt.tokens);
    }
    auto r = Row();
    auto cursor = Cursor(table);
    std::memcpy(&r, cursor.value(), sizeof(Row));
    assert(r.id == 1);
    assert((compareStringToChar(r.username, "test")));
    assert((compareStringToChar(r.email, "test@email.com")));

    cursor.advance();
    std::memcpy(&r, cursor.value(), sizeof(Row));
    assert(r.id == 2);
    assert(compareStringToChar(r.username, "test2"));
    assert(compareStringToChar(r.email, "test2@email.com"));
}

void test_persistence(){
    // Clear contents
    std::ofstream f("./test_db", std::ofstream::trunc);
    f.close();
    auto table = new Table("test_db");
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

    // TODO: resolve bug introduced in persistence, inserting into pre-existing database fails
    auto table2 = new Table("test_db");
    auto r = Row();
    auto cursor = Cursor(table2);
    std::memcpy(&r, cursor.value(), sizeof(Row));
    assert(r.id == 1);
    assert((compareStringToChar(r.username, "test")));
    assert((compareStringToChar(r.email, "test@email.com")));
    cursor.advance();
    std::memcpy(&r, cursor.value(), sizeof(Row));
    assert(r.id == 2);
    assert(compareStringToChar(r.username, "test2"));
    assert(compareStringToChar(r.email, "test2@email.com"));
}

int main() {
    test_insert();
    test_persistence();
    std::cout << "\033[32m------All tests passed------\033[0m\n";
    return 0;
}
