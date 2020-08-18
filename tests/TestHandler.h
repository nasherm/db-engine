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
    std::vector<std::string> testNames;
    unsigned int numberOfFailedTests;
public:
    void logFailure(std::string failMessage, std::string testName);
    void addTest(TestSignature t, std::string name=""){
        tests.push_back(t);
        testNames.push_back(name);
    };
    void runTests(){
        std::string testReturnMessage;
        for (auto i = 0 ; i < tests.size(); i++) {
//            auto testName = t.operator()();
            testReturnMessage = tests[i]();
            if (testReturnMessage.size() > 0) {
                logFailure(testReturnMessage, testNames[i]);
            }
        }
    }

    std::vector<std::string> failLogs;
};
#endif //DB_ENGINE_TESTHANDLER_H
