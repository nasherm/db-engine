//
// Created by potato on 06/06/2020.
//

#include "Repl.h"
#include <iostream>
#include <vector>

void tokenize(std::string in, const std::string& delim, std::vector<std::string> &tokens) {
    size_t pos = 0;
    std::string token;
    while ((pos = in.find(delim)) != std::string::npos) {
        token = in.substr(0, pos);
        tokens.push_back(token);
        in.erase(0, pos + delim.length());
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
    tokenize(in, " ", statement.tokens);
    statement.command = stringToCommand(statement.tokens[0]);
}

void Repl::start() {
    Statement stmt;
    std::string in;
    while(true) {
        std::cout << "+> ";
        readLine(stmt);
        if (stmt.command == Command::Quit) {
            std::cout << "Goodbye\n";
            break;
        }
        else if (stmt.command == Command::Failed)
            std::cout << "Couldn't read command\n";
        else {
            executeStatement(stmt);
        }
    }
}

void Repl::executeStatement(const Statement &stmt) {
    return;
}