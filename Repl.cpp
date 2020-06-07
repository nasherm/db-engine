//
// Created by potato on 06/06/2020.
//

#include "Repl.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <sstream>

void tokenize(std::string in, const char delim, std::vector<std::string> &tokens) {
    std::istringstream iss(in);
    std::string str;
    while (getline(iss, str, delim)){
        tokens.push_back(str);
    }
}

Command stringToCommand(const std::string& comm){
    if (comm == ":q") return Command::Quit;
    if (comm == "insert") return Command::Insert;
    if (comm == "select") return Command::Select;
    if (comm == ".tables") return Command::Tables;
    if (comm == "delete") return Command::Delete;
    return Command::Failed;
}

void Repl::readLine(Statement& statement) {
    std::string in;
    std::getline(std::cin, in);
    tokenize(in, ' ', statement.tokens);
    statement.command = stringToCommand(statement.tokens[0]);
}

void Repl::start() {
    table = new Table;
    while(true) {
        Statement stmt;
        std::cout << "+>";
        readLine(stmt);
        if (stmt.command == Command::Quit) {
            std::cout << "Goodbye\n";
            break;
        }
        else if (stmt.command == Command::Failed) {
            std::cout << "Couldn't read command\n";
        }
        else {
            try{
                executeStatement(stmt);
            } catch(std::exception& e) {
                std::cout << "Failed to execute command: " << e.what() << std::endl;
            }
        }
    }
    delete table;
}

void Repl::executeStatement(const Statement &stmt) {
    if (stmt.command == Command::Insert) {
        assert(stmt.tokens.size() == 4);
        assert(table->spaceAvail());
        table->insert(stmt.tokens);
    }
    else if (stmt.command == Command::Select) {
        assert(stmt.tokens.size() == 1);
        table->select();
    }
}