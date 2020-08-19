//
// Created by nashe mncube on 17/08/2020.
//

#include "Schema.h"
#include "Util.h"
//#include "Util.h"



Schema::Schema(std::vector<ColumnSchema>&columnSchemas){
    // 1.Check size limits
    // 2. Set variables
    // 3.???
    if (columnSchemas.size() * sizeof(ColumnSchema) > MAX_ROW_BYTE_COUNT) {
        std::cout << "Too many columns\n";
        return;
    }
    COLUMN_COUNT = columnSchemas.size();
    for (auto c : columnSchemas) {
        ROW_WIDTH += c.columnDataType.getSize();
    }
    columns= columnSchemas;
}

void Schema::constructColumnSchemaFromString(std::string& in, ColumnSchema& dest) {
    // Tuples should be of the form (name, type)
    auto tokens = std::vector<std::string>();
    Util::tokenize(in, ',', tokens);
    if (tokens.size() != 2) {
        Util::failWithMessageForHandledError("Incorrect tuple definition: " + in);
    }
    else{
        std::string name, type;
        Util::regexSearch(tokens[0], name, RegexType::RegexString);
        Util::regexSearch(tokens[1], type, RegexType::RegexString);
        memcpy(dest.columnName, name.c_str(), 256);
        dest.columnDataType = matchDataType(type);
    }
}

void Schema::printSchema() {
    std::cout << "ROW WIDTH(bytes): " << ROW_WIDTH << std::endl;
    std::cout << "COLUMN COUNT: " << COLUMN_COUNT << std::endl;
    std::cout << "-------------COLUMNS------------\n";
    for (auto c : columns) {
        std::cout
        << "("
        << c.columnName
        << ','
        << c.columnDataType.toString()
        << ")\n";
    }
}

