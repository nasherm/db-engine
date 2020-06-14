//
// Created by potato on 06/06/2020.
//

#include "Table.h"
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <zconf.h>
#include <iostream>

void printConstants() {
    printf("ROW_SIZE: %d\n", sizeof(Row));
    printf("COMMON_NODE_HEADER_SIZE: %d\n", COMMON_NODE_HEADER_SIZE);
    printf("LEAF_NODE_HEADER_SIZE: %d\n", LEAF_NODE_HEADER_SIZE);
    printf("LEAF_NODE_CELL_SIZE: %d\n", LEAF_NODE_CELL_SIZE);
    printf("LEAF_NODE_SPACE_FOR_CELLS: %d\n", LEAF_NODE_SPACE_FOR_CELLS);
    printf("LEAF_NODE_MAX_CELLS: %d\n", LEAF_NODE_MAX_CELLS);
}

void printTree(Node* node) {
    uint32_t num_cells = leafNodeNumCells(node);
    printf("leaf (size %d)\n", num_cells);
    for (uint32_t i = 0; i < num_cells; i++) {
        auto key = leafNodeCellKey(node, i);
        printf("  - %d : %d\n", i, *key);
    }
}

uint32_t leafNodeNumCells(Node* node) {
    return node->numCells;
}

uint8_t* leafNodeCell(Node* node, uint32_t cellNum) {
    return &(node->cells[LEAF_NODE_CELL_SIZE * cellNum]);
}

uint32_t* leafNodeCellKey(Node*  node, uint32_t cellNum){
    return (uint32_t*) leafNodeCell(node, cellNum);
}

uint8_t* leafNodeCellValue(Node* node, uint32_t cellNum) {
    return leafNodeCell(node, cellNum) + LEAF_NODE_KEY_SIZE;
}

void incrementNumCells(Node* node) {
    node->numCells += 1;
}

void setKey(Node* node, uint32_t cellNum, uint32_t key){
    *leafNodeCellKey(node, cellNum) = key;
}

void initNode(Node* node) {
    node->numCells = 0;
    node->type = NodeLeaf;
    node->parent = nullptr;
}

void Pager::pagerOpen(const std::string& fileName) {
    int fd = open(fileName.c_str(), O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
    if (fd == -1) {
        printf("Unable to print file: %s\n", fileName.c_str());
        throw std::runtime_error("file failure");
    }
    fileDescriptor = fd;
    fileLength = lseek(fd, 0, SEEK_END);
    pagesCount = fileLength / PAGE_SIZE;
    if (fileLength % PAGE_SIZE) {
        throw std::runtime_error("Corrupt file, database isn't a whole number of pages");
    }
}

Node* Pager::getPage(const uint32_t pageNum) {
    if (pageNum > MAX_PAGES) {
        throw std::runtime_error("Tried to get page out of bounds");
    }

    if (pages[pageNum] == nullptr){
        // Cache miss
        pages[pageNum] = new Node;
        initNode(pages[pageNum]);
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
        if (pageNum >= pagesCount) {
            pagesCount = pageNum + 1;
        }
    }
    return pages[pageNum];
}

void Pager::flushPage(const uint32_t pageIndex) {
    if (pages[pageIndex] == nullptr) {
        std::cout << "Trying to flush null page\n";
        return;
    }
    auto fileOffset = lseek(fileDescriptor, pageIndex * PAGE_SIZE, SEEK_SET);
    if(fileOffset == -1) {
        std::cout << "Seek error during flush\n";
        return;
    }
    auto bytesWritten = write(fileDescriptor, pages[pageIndex], PAGE_SIZE);
    if (bytesWritten == -1) {
        std::cout << "Error writing page to file\n";
    }
}

void copyString(char* dest, const std::string& src){
    for (size_t i = 0; i < src.length(); i++){
        dest[i] = src[i];
    }
}

void Table::insert(const std::vector<std::string> &args) {
    auto node = getPage(rootPageNum);
    if (leafNodeNumCells(node) >= LEAF_NODE_MAX_CELLS){
        throw std::runtime_error("No space in table for insert");
    }

    auto row = new Row;
    row->id = std::stoi(args[1]);
    copyString(row->username, args[2]);
    copyString(row->email, args[3]);

    auto cursor = new Cursor(this);
    cursor->tableFind(row->id);
    if (cursor->getCellNum() < leafNodeNumCells(node)){
        auto keyAtIndex = leafNodeCellKey(node, cursor->getCellNum());
        if (*keyAtIndex == row->id) {
            throw std::runtime_error("Duplicate key");
        }
    }
    cursor->leafNodeInsert(row->id, row);
}

void printRow(const Row* row) {
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

void Table::select(){
    auto row = new Row;
    auto cursor = new Cursor(this);
    while (!(cursor->atEndOfTable())) {
        auto slot = cursor->value();
        std::memcpy(row, slot, sizeof(Row));
        printRow(row);
        cursor->advance();
    }
}

void Table::tableClose() {
    for (auto i = 0; i < pager->getPagesCount(); i++) {
        if (pager->pages[i] == nullptr) {
            continue;
        }
        pager->flushPage(i);
    }

    if (close(pager->fileDescriptor) == -1) {
        throw std::runtime_error ("Failed to close db\n");
    }
}

uint8_t* Cursor::value() {
    try {
        return leafNodeCellValue(table->getPage(pageNum), cellNum);
    } catch (std::exception& e) {
        std::cout << "failed to get page, exception thrown:"
                  << e.what()
                  << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Cursor::advance() {
    auto node = table->getPage(pageNum);
    cellNum ++;
    if (cellNum >= leafNodeNumCells(node)){
        endOfTable = true;
    }
}

void Cursor::leafNodeInsert(uint32_t key, Row* value) {
    auto node = table->getPage(pageNum);
    auto numCells = leafNodeNumCells(node);
    if (numCells >= LEAF_NODE_MAX_CELLS){
        std::cout << "TODO: splitting\n";
        return;
    }

    if (cellNum < numCells) {
        for (auto i = numCells; i > cellNum; i--){
            memcpy(leafNodeCell(node, i), leafNodeCell(node, i-1), LEAF_NODE_CELL_SIZE);
        }
    }
    incrementNumCells(node);
    setKey(node, cellNum, key);
    memcpy(leafNodeCellValue(node, cellNum), value, sizeof(Row));
}
void Cursor::leafNodeFind(uint32_t pageNum, uint32_t key) {
    auto node = table->getPage(pageNum);
    auto numCells = node->numCells;
    this->pageNum = pageNum;
    endOfTable = true;

    // Binary search
    uint32_t minIndex = 0;
    uint32_t onePastMaxIndex = numCells;
    while(onePastMaxIndex != minIndex){
        auto index = (minIndex + onePastMaxIndex)/2;
        auto keyAtIndex = *leafNodeCellKey(node, index);
        if (keyAtIndex == key) {
            cellNum = index;
            return;
        }
        if (key < keyAtIndex) {
            onePastMaxIndex = index;
        } else{
            minIndex = index+1;
        }
    }
    cellNum = minIndex;
}

void Cursor::tableFind(uint32_t key) {
    auto rootPageNum = table->getRootPageNum();
    auto node = table->getPage(rootPageNum);
    if (node->type == NodeLeaf) {
        this->leafNodeFind(rootPageNum, key);
    } else {
        printf("todo\n");
    }
}

