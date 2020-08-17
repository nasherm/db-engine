//
// Created by nashe mncube on 17/08/2020.
//

#ifndef DB_ENGINE_UTIL_H
#define DB_ENGINE_UTIL_H

#include <sstream>
#include <vector>
#include <regex>
#include <iostream>

typedef enum RegexType{
    RegexInt,
    RegexString,
} RegexType;

class Util {
public:
    Util() = default;
    static bool compareStringToChar(const char *c, const std::string& s) {
        for (auto i = 0; i < s.length(); i++) {
            if (c[i] == '\0' || c[i] != s[i])
                return false;
        }
        return true;
    }

    static void tokenize(std::string in, const char delim, std::vector<std::string> &tokens) {
        std::istringstream iss(in);
        std::string str;
        while (getline(iss, str, delim)){
            tokens.push_back(str);
        }
    }

    static void failWithMessageForHandledError(const std::string& message) {
        std::cout << message << std::endl;
        throw std::runtime_error(message);
    }


    static std::regex regexType(RegexType regex){
        switch (regex){
            case RegexInt:
                return std::regex("[0-9]+");
            case RegexString:
                return std::regex("[A-Za-z]+");
            default:
                return std::regex();
        }
    }

    static void regexSearch(std::string& src, std::string& dest, RegexType regex){
        auto regexPattern = regexType(regex);
        std::smatch matches;
        std::regex_search(src, matches, regexPattern);
        if (!matches.empty()){
            // Simply return the first match for now
            dest = matches[0];
        }
    }

};

#endif //DB_ENGINE_UTIL_H
