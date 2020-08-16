//
// Created by nashe mncube on 16/08/2020.
//

#ifndef DB_ENGINE_TABLE_H
#define DB_ENGINE_TABLE_H

#include <string>

class Table {
private:

public:
    Table() = default;
    Table(std::string) {}
    void tableClose() {}
};


#endif //DB_ENGINE_TABLE_H
