//
// Created by nashe mncube on 16/08/2020.
//

#include "Table.h"

void Table::configSchemaFromInput(std::vector<std::string>& in) {
    std::vector<ColumnSchema> columnSchemas;
    for (auto s: in){
        try {
            ColumnSchema columnSchema;
            Schema::constructColumnSchemaFromString(s, columnSchema);
            columnSchemas.push_back(columnSchema);
        } catch (std::exception e){
            // For now errors in schema definition are ignored
        }
    }
    schema = Schema(columnSchemas);
}
