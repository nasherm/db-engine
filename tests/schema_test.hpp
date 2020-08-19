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
    if (c.columnDataType.getType() != expectedDataType.getType()) {
        errorMessage += "\nInput: " + input;
        errorMessage +=
                "\nData type mismatch, expected \"" + expectedDataType.getType() + "\" got \""
                + c.columnDataType.toString() + "\"";

    }
    if (c.columnName != expectedLabel) {
        errorMessage += "\nInput: " + input;
        errorMessage +=
                "\nColumn name mismatch, expected \"" + expectedLabel
                + "\" got " + c.columnName + "\"";
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
    checkColumnSchema(DataType("string"), "name",
                      columnSchemas[0], errorMessage, inputs[0]);
    checkColumnSchema(DataType("string"), "address",
                      columnSchemas[1], errorMessage, inputs[1]);
    checkColumnSchema(DataType("int"), "height",
                      columnSchemas[2], errorMessage, inputs[2]);
    return errorMessage;
}


