//
// Created by potato on 06/06/2020.
//

#ifndef DB_ENGINE_REPL_H
#define DB_ENGINE_REPL_H

#include <string>
#include <vector>
#include "Table.h"
enum Command {Insert, Select, Tables, Delete, Quit, Failed};

typedef struct {
    Command command;
    std::vector<std::string> tokens;
} Statement;

class Repl {
    Table *table;
public:
    Repl() = default;
    static void readLine(Statement&);
    void start();
    void executeStatement(const Statement& stmt);
};


#endif //DB_ENGINE_REPL_H
