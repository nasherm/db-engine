//
// Created by potato on 06/06/2020.
//

#ifndef DB_ENGINE_REPL_H
#define DB_ENGINE_REPL_H

#include <string>
#include <vector>
#include "Table.h"
enum Command {
    Insert, // Insert data into db
    Select, // Select data
    Tables, // Print tables
    Delete, // Delete based on id
    BTree, // Print BTree
    Schema, // Define the database schema with a list of pairs (column_name, column_size(bytes))
    Quit, // Exit
    Show, // Print schema
    Failed, // Returned if command unreadable
    Name, // Show name of DB
};

typedef struct {
    Command command;
    std::vector<std::string> tokens;
} Statement;

class Repl {
    std::string fileName = "db_out";
    Table *table;
public:
    Repl() {
        table = new Table(this->fileName);
    }
    explicit Repl(char* fileName): fileName(fileName) {
        table = new Table(this->fileName);
    }
    static void stmtFromString(std::string& in, Statement&);
    void start();
    void executeStatement(const Statement& stmt);
};


#endif //DB_ENGINE_REPL_H
