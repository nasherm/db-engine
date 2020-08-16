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

}

void test_persistence(){
    // Clear contents
}

int main() {
    test_insert();
    test_persistence();
    std::cout << "\033[32m------All tests passed------\033[0m\n";
    return 0;
}
