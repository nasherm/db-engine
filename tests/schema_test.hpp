//
// Created by nashe mncube on 17/08/2020.
//
#include "../src/Schema.h"
#include <iostream>
void checkColumnSchema(
        DataType expectedDataType,
        std::string expectedLabel,
        ColumnSchema c,
        std::string& errorMessage,
        std::string& input) {
    if (c.columnDatatype != expectedDataType) {
        errorMessage += "\nInput: " + input;
        errorMessage +=
                "\nData type mismatch, expected string got"
                + Schema::dataTypeToString(c.columnDatatype);

    }
    if (c.columnName != expectedLabel) {
        errorMessage += "\Input: " + input;
        errorMessage +=
                "\nColumn name mismatch, expected \"" + expectedLabel
                + "\" got" + c.columnName;
    }
}
std::string testConstructSchemaTriple(){
    std::vector<std::string> inputs;
    inputs.push_back("(name, string)");
    inputs.push_back("(address, string)");
    inputs.push_back("(height, int)");
    std::vector<ColumnSchema> columnSchemas(3, ColumnSchema());
    std::string errorMessage;
    for (auto i = 0; i < inputs.size(); i++){
        Schema::constructColumnSchemaFromString(inputs[i], columnSchemas[i]);
    }
    checkColumnSchema(DataType::String, "name",
                      columnSchemas[0], errorMessage, inputs[0]);
    checkColumnSchema(DataType::String, "address",
                      columnSchemas[1], errorMessage, inputs[1]);
    checkColumnSchema(DataType::Int, "height",
                      columnSchemas[2], errorMessage, inputs[2]);
    return errorMessage;
}


