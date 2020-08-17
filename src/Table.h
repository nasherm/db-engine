//
// Created by nashe mncube on 16/08/2020.
//

#ifndef DB_ENGINE_TABLE_H
#define DB_ENGINE_TABLE_H

#include <string>
#include <vector>
#include "Schema.h"
class Table {
private:
    std::string dbFileName;
    Schema schema;
public:
    Table() = default;
    Table(std::string fileName): dbFileName(fileName){}
    void configSchemaFromInput(std::vector<std::string>);
    void tableClose() {}
};


#endif //DB_ENGINE_TABLE_H
