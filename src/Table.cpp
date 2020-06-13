//
// Created by potato on 06/06/2020.
//

#include "Table.h"
#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <zconf.h>
#include <iostream>

void Pager::pagerOpen(const std::string& fileName) {
    int fd = open(fileName.c_str(), O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
    if (fd == -1) {
        printf("Unable to print file: %s\n", fileName.c_str());
        throw std::runtime_error("file failure");
    }
    fileDescriptor = fd;
    fileLength = lseek(fd, 0, SEEK_END);
}

uint8_t* Pager::getPage(const uint32_t pageNum) {
    if (pageNum > MAX_PAGES) {
        throw std::runtime_error("Tried to get page out of bounds");
    }

    if (pages[pageNum] == nullptr){
        // Cache miss
        pages[pageNum] = new uint8_t[PAGE_SIZE];
        auto numPages = fileLength / PAGE_SIZE;

        // In the case where we can save a partial page
        if (fileLength % PAGE_SIZE) numPages ++;

        if (pageNum <= numPages) {
            lseek(fileDescriptor, pageNum * PAGE_SIZE, SEEK_SET);
            auto bytesReadFromFile = read(fileDescriptor, pages[pageNum], PAGE_SIZE);
            if (bytesReadFromFile == -1) {
                throw std::runtime_error("Couldn't read page from file");
            }
        }

    }
    return pages[pageNum];
}

uint8_t* Table::rowSlot(const uint32_t& rowNum) {
    uint32_t pageNum = rowNum / ROWS_PER_PAGE;
    auto rowOffset = (rowNum % ROWS_PER_PAGE) * sizeof(Row);
    return pager->getPage(pageNum) + rowOffset;
}

void copyString(char* dest, const std::string& src){
    for (size_t i = 0; i < src.length(); i++){
        dest[i] = src[i];
    }
}

void Table::insert(const std::vector<std::string> &args) {
    try {
        Row *row = new Row;
        auto slot = rowSlot(rowCount);
        row->id = std::stoi(args[1]);
        copyString(row->username, args[2]);
        copyString(row->email, args[3]);
        std::memcpy(slot, row, sizeof(Row));
        rowCount++;
    } catch (std::exception& e) {
        std::cout << "Failed to insert to table exception: "
        << e.what()
        << std::endl;
    }
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