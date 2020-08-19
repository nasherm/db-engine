//
// Created by nashe mncube on 16/08/2020.
//

#ifndef DB_ENGINE_TABLE_H
#define DB_ENGINE_TABLE_H

#include <string>
#include <utility>
#include <vector>
#include "Schema.h"
class Table {
private:
    std::string dbFileName;
    Schema schema;
public:
//    Table() = default;
    explicit Table(std::string fileName="db_out"): dbFileName(std::move(fileName)){
        // TODO: check db exits -> if so, load from disk -> then init schema from metadata
    }
    void configSchemaFromInput(std::vector<std::string>&);
    void saveToDisk() {}
    void printSchema(){
        schema.printSchema();
    }
};


#endif //DB_ENGINE_TABLE_H
