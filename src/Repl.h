//
// Created by potato on 06/06/2020.
//

#ifndef DB_ENGINE_REPL_H
#define DB_ENGINE_REPL_H

#include <string>
#include <vector>
#include "Table.h"
enum Command {Insert, Select, Tables, Delete, Quit, Failed, Constants, BTree};

typedef struct {
    Command command;
    std::vector<std::string> tokens;
} Statement;

class Repl {
    std::string fileName = "db_out";
    Table *table;
public:
    Repl() = default;
    explicit Repl(char* fileName) {
        this->fileName = fileName;
    }
    static void stmtFromString(std::string& in, Statement&);
    void start();
    void executeStatement(const Statement& stmt);
};


#endif //DB_ENGINE_REPL_H
