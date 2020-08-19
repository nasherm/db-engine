//
// Created by nashe mncube on 17/08/2020.
//

#ifndef DB_ENGINE_SCHEMA_H
#define DB_ENGINE_SCHEMA_H

#include <stdint.h>
#include <vector>
#include <string>
//#include "Util.h"

const uint16_t MAX_ROW_BYTE_COUNT = 4096;

//typedef enum DataType {
//    String, // max 256 bytes
//    Int, // max 4 bytes
//    Undefined, // Errors
//}DataType;

class DataType {
private:
    std::string type = "undefined";
    unsigned int size = 0;
public:
    DataType() = default;
   explicit DataType(std::string type): type(type){
        if (type == "string")
            size = 256;
        else if (type == "int")
            size = 4;
        else
            type = "undefined"; // wrong arg
    }
    std::string toString(){
        return type;
    }
    unsigned int getSize() {
        return size;
    }

    std::string getType(){
       return type;
   }
};

// Input triples when defining schema are tuples of (name, type) which define a column
//typedef struct ColumnSchema {
//} ColumnSchema;

class ColumnSchema {
public:
    std::string columnName;
    DataType columnDataType = DataType();
    ColumnSchema() = default;
};

class Schema {
private:
    uint16_t ROW_WIDTH = 0;
    uint16_t COLUMN_COUNT = 0;
    // Column at index i has offset stored in container
    std::vector<ColumnSchema> columns;
public:
    Schema() = default;
    explicit Schema(std::vector<ColumnSchema>& columnSchemas);
    static DataType matchDataType(std::string s) {
        return DataType(s);
    }
    static void constructColumnSchemaFromString(std::string& in, ColumnSchema& dest);
    void printSchema();
    void serializeMetaData(uint8_t* dest);
    void readMetaData(uint8_t* src);
};


#endif //DB_ENGINE_SCHEMA_H
