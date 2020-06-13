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

static const uint32_t PAGE_SIZE = 4096;
static const uint32_t MAX_PAGES = 100;

class Pager{
public:
    Pager() = default;
    ~Pager() {
        for (auto page: pages){
            delete [] page;
        }
    }
    void pagerOpen(const std::string& fileName);
    uint8_t* getPage(uint32_t pageNum);
    void flushPage(uint32_t pageIndex, uint32_t bytes);

    int fileDescriptor;
    uint32_t fileLength;
    std::array<uint8_t*, MAX_PAGES> pages;
};

// Data will be stored in pages
class Table {
private:
    static const uint32_t ROWS_PER_PAGE = PAGE_SIZE / sizeof(Row);
    static const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * MAX_PAGES;
    Pager* pager;
    uint32_t rowCount = 0;
public:
    Table() {
        pager = new Pager;
        pager->pagerOpen("db_out");
    }
    explicit Table(const std::string& fileName) {
        pager = new Pager;
        pager->pagerOpen(fileName);
        rowCount = pager->fileLength / sizeof(Row);
    }
    ~Table() {
        delete pager;
    }

    [[nodiscard]] bool spaceAvail() const { return (rowCount + 1 < TABLE_MAX_ROWS); }
    uint8_t *rowSlot(const uint32_t& rowNum);
    void insert(const std::vector<std::string>& args);
    void select();
    static bool compareStringToChar(const char *c, const std::string& s){
        for (auto i = 0; i < s.length(); i++){
            if (c[i] == '\0' || c[i] != s[i])
                return false;
        }

        return true;
    }
    void tableClose();
};


#endif //DB_ENGINE_TABLE_H
