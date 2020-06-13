//
// Created by potato on 06/06/2020.
//

#include "Repl.h"
#include <iostream>
#include <vector>
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

void Repl::stmtFromString(std::string& in, Statement& statement) {
    tokenize(in, ' ', statement.tokens);
    statement.command = stringToCommand(statement.tokens[0]);
}

void Repl::start() {
    table = new Table(fileName);
    std::string in;
    while(true) {
        Statement stmt;
        std::cout << "+>";
        std::getline(std::cin, in);
        stmtFromString(in,stmt);
        if (stmt.command == Command::Quit) {
            table->tableClose();
            std::cout << "Goodbye\n";
            break;
        }
        else if (stmt.command == Command::Failed) {
            std::cout << "Couldn't read command\n";
        }
        else {
            try{
                executeStatement(stmt);
                std::cout << "Executed\n";
            } catch(std::exception& e) {
                std::cout
                << "Failed to execute command: "
                << e.what()
                << std::endl;
            }
        }
    }
    delete table;
}

void Repl::executeStatement(const Statement &stmt) {
    if (stmt.command == Command::Insert) {
        if (stmt.tokens.size() != 4){
            throw std::runtime_error("Incorrect number of args for insert");
        }
        if (!table->spaceAvail()){
            throw std::runtime_error("No space in table for insert");
        }
        table->insert(stmt.tokens);
    }
    else if (stmt.command == Command::Select) {
        if (stmt.tokens.size() != 1){
            throw std::runtime_error("Incorrect number of args for select");
        }
        table->select();
    }

}