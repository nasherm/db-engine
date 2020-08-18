//
// Created by potato on 07/06/2020.
//

#include <iostream>
#include "TestHandler.h"

void TestHandler::logFailure(std::string message, std::string testName){
    failLogs.push_back("Test name: " + testName + "\nFailure: " +message);
    numberOfFailedTests += 1;
}

int main() {
    auto test = TestHandler();
    test.addTest(testConstructSchemaTriple, "Construct Schema Triple");

    test.runTests();

    if (test.failLogs.size() > 0){
        std::cout << "\x1b[31m-------Some tests failed-----\033[0m\n";
        for (auto i = 0; i < test.failLogs.size(); i++) {
            std::cout << i+1 << "\t\t" << test.failLogs[i] << std::endl;
        }
    }
    else{
        std::cout << "\033[32m------All tests passed------\033[0m\n";
    }
    return 0;
}
