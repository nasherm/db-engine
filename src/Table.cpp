//
// Created by potato on 06/06/2020.
//

#include "Table.h"
#include <cassert>
#include <cstring>

uint8_t* Table::rowSlot(const uint32_t& rowNum) {
    uint32_t pageNum = rowNum / ROWS_PER_PAGE;
    if (pages[pageNum] == nullptr){
        pages[pageNum] = new uint8_t[PAGE_SIZE];
    }
    auto rowOffset = (rowNum % ROWS_PER_PAGE) * sizeof(Row);
    return pages[pageNum] + rowOffset;
}

void copyString(char* dest, const std::string& src){
    for (size_t i = 0; i < src.length(); i++){
        dest[i] = src[i];
    }
}

void Table::insert(const std::vector<std::string> &args) {
    Row *row = new Row;
    auto slot = rowSlot(rowCount);
    row->id = std::stoi(args[1]);
    copyString(row->username, args[2]);
    copyString(row->email, args[3]);
    std::memcpy(slot, row, sizeof(Row));
    rowCount ++;
}

void printRow(const Row* row) {
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

void Table::select(){
    auto row = new Row;
    for (auto i = 0; i < rowCount; i++) {
        auto slot = rowSlot(i);
        std::memcpy(row, slot, sizeof(Row));
        printRow(row);
    }
}