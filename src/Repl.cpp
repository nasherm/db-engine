//
// Created by Nasherm on 06/06/2020.
//

#include "Repl.h"
#include "Util.h"
#include <iostream>
#include <vector>
//#include "Util.h"

Command stringToCommand(const std::string& comm){
    if (comm == ":q") return Command::Quit;
    if (comm == "insert") return Command::Insert;
    if (comm == "select") return Command::Select;
    if (comm == ".tables") return Command::Tables;
    if (comm == "delete") return Command::Delete;
    if (comm == ".btree") return Command::BTree;
    if (comm == "schema") return Command::Schema;
    if (comm == ".show") return Command::Show;
    if (comm == ".") return Command::Name;
    return Command::Failed;
}

void Repl::stmtFromString(std::string& in, Statement& statement) {
    Util::tokenize(in, ' ', statement.tokens);
    statement.command = stringToCommand(statement.tokens[0]);
}

void Repl::start() {
    std::string in;
    auto quit = false;
    do {
        Statement stmt;
        std::cout << "+>";
        std::getline(std::cin, in);
        stmtFromString(in, stmt);
        if (stmt.command == Command::Quit) {
            table.tableClose();
            std::cout << "Goodbye\n";
            quit = true;
        } else if (stmt.command == Command::Name) {
            std::cout << "DB Name: " << fileName << std::endl;
        } else if (stmt.command == Command::Schema){
            table.configSchemaFromInput(stmt.tokens);
        }
        else
        {
            std::cout << "Couldn't read command\n";
        }
    } while (!quit);
}

void Repl::executeStatement(const Statement &stmt) {
//    if (stmt.command == Command::Insert) {
//        if (stmt.tokens.size() != 4){
//            throw std::runtime_error("Incorrect number of args for insert");
//        }
////        table->insert(stmt.tokens);
//    }
//    else if (stmt.command == Command::Select) {
//        if (stmt.tokens.size() != 1){
//            throw std::runtime_error("Incorrect number of args for select");
//        }
////        table->select();
//    }
}