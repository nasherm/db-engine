//
// Created by nashe mncube on 17/08/2020.
//
#include "../src/Schema.h"

std::string testConstructSchemaTriple(){
    std::vector<std::string> inputs;
    inputs.push_back("(name, string)");
    inputs.push_back("(address, string)");
    inputs.push_back("(height, int)");
    std::vector<ColumnSchema> columnSchemas(3, ColumnSchema());
    for (auto i = 0; i < inputs.size(); i++){
        Schema::constructColumnSchemaFromString(inputs[i], columnSchemas[i]);
    }

    return "";
}


