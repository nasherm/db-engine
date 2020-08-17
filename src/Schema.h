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

typedef enum DataType {
    String, // max 256 bytes
    Int, // max 4 bytes
    Undefined, // Errors
}DataType;



// Input triples when defining schema are tuples of (name, type) which define a column
typedef struct ColumnSchema {
    char columnName[256];
    DataType columnDatatype;
} ColumnSchema;

class Schema {
private:
    uint16_t ROW_WIDTH;
    uint16_t ROW_BYTE_COUNT;
    uint16_t COLUMN_COUNT;
    // Column at index i has offset stored in container
    std::vector<uint16_t> COLUMN_OFFSETS;
public:
    Schema() = default;
    Schema(std::vector<ColumnSchema> columnSchemas);
    static DataType matchDataType(std::string s) {
        if (s == "string") return DataType::String;
        if (s == "int") return DataType::Int;
        return DataType::Undefined;
    }
    void serializeMetaData(uint8_t* dest);
    void readMetaData(uint8_t* src);
    static void constructColumnSchemaFromString(std::string& in, ColumnSchema& dest);
};


#endif //DB_ENGINE_SCHEMA_H
