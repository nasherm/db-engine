//
// Created by nashe mncube on 17/08/2020.
//

#ifndef DB_ENGINE_TESTHANDLER_H
#define DB_ENGINE_TESTHANDLER_H
#include <string>
#include <vector>
#include <functional>
#include "schema_test.hpp"
#include "../src/Repl.h"
#include "../src/Table.h"
#include "../src/Util.h"
#include "../src/Schema.h"

typedef std::function<std::string()> TestSignature;
class TestHandler {
private:
    std::vector<TestSignature> tests;
    unsigned int numberOfFailedTests;
public:
    void logFailure(std::string failMessage);
    void addTest(TestSignature t){
        tests.push_back(t);
    };
    void runTests(){
        std::string testReturnMessage;
        for (auto t : tests) {
            testReturnMessage = t();
            if (testReturnMessage.size() > 0) {
                logFailure(testReturnMessage);
            }
        }
    }

    std::vector<std::string> failLogs;
};
#endif //DB_ENGINE_TESTHANDLER_H
