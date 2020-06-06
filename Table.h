//
// Created by potato on 06/06/2020.
//

#ifndef DB_ENGINE_TABLE_H
#define DB_ENGINE_TABLE_H

#include <stdint.h>
#include <vector>
#include <string>
#include <array>
// Database schema with number of bytes per column
// id - integer(4)
// username - varChar(32)
// email - varchar(255)
#define USER_LEN 32
#define EMAIL_LEN 255
typedef struct {
    uint32_t id;
    char username[USER_LEN];
    char email[EMAIL_LEN];
}Row;
// Data will be stored in pages
class Table {
private:
    static const uint32_t PAGE_SIZE = 4096;
    static const uint32_t MAX_PAGES = 100;
    static const uint32_t ROWS_PER_PAGE = PAGE_SIZE / sizeof(Row);
    static const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * MAX_PAGES;
    std::array<uint8_t*, MAX_PAGES> pages;
    uint32_t rowCount;
public:
    Table() = default;
    bool spaceAvail() { return (rowCount + 1 < MAX_PAGES); }
    uint8_t *rowSlot(const uint32_t& rowNum);
    void insert(const std::vector<std::string>& args);
    void select();
};


#endif //DB_ENGINE_TABLE_H
