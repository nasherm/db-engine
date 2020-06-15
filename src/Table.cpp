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
    return *((uint32_t*)node->data);
}

uint8_t* leafNodeCell(Node* node, uint32_t cellNum) {
    return &(node->data[LEAF_NODE_NUM_CELLS_SIZE + (LEAF_NODE_CELL_SIZE * cellNum)]);
}

uint32_t* leafNodeCellKey(Node*  node, uint32_t cellNum){
    return (uint32_t*) leafNodeCell(node, cellNum);
}

uint8_t* leafNodeCellValue(Node* node, uint32_t cellNum) {
    return leafNodeCell(node, cellNum) + LEAF_NODE_KEY_SIZE;
}

void incrementNumCells(Node* node) {
    *((uint32_t*)node->data)+= 1;
}

void setNumCells(Node* node, uint32_t v) {
    *((uint32_t*)node->data) = v;
}
void setKey(Node* node, uint32_t cellNum, uint32_t key){
    *leafNodeCellKey(node, cellNum) = key;
}

void initLeafNode(Node* node) {
    setNumCells(node, 0);
    node->type = NodeLeaf;
    node->parent = nullptr;
}

uint32_t* internalNodeNumKeys(Node* node) {
    return (uint32_t*)node->data;
}

void initInternalNode(Node* node) {
    node->type = NodeInternal;
    node->isRoot = false;
    *internalNodeNumKeys(node) = 0;
}

uint32_t* internalNodeRightChild(Node* node) {
    return (uint32_t*)(&node->data[INTERNAL_NODE_NUM_KEYS_SIZE]);
}

uint32_t* internalNodeCell(Node* node, uint32_t cellNum) {
    return (uint32_t*)(&node->data[
            INTERNAL_NODE_RIGHT_CHILD_SIZE
            + INTERNAL_NODE_NUM_KEYS_SIZE
            + (cellNum * INTERNAL_NODE_CELL_SIZE)]);
}

uint32_t* internalNodeChild(Node* node, uint32_t childNum) {
    auto numKeys = *internalNodeNumKeys(node);
    if (childNum > numKeys) {
        throw std::runtime_error("childNum greater that number of available keys");
    } else if (childNum == numKeys) {
        return internalNodeRightChild(node);
    } else{
        return internalNodeChild(node, childNum);
    }
}

uint32_t* internalNodeCellKey(Node* node, uint32_t keyNum) {
    return internalNodeCell(node, keyNum) + INTERNAL_NODE_CHILD_SIZE;
}

uint32_t getNodeMaxKey(Node* node) {
    if(node->type == NodeLeaf) {
        return *leafNodeCellKey(node, leafNodeNumCells(node) - 1);
    } else {
        return *internalNodeCellKey(node, *internalNodeNumKeys(node) - 1);
    }
}

void indent(uint32_t level) {
  for (uint32_t i = 0; i < level; i++) {
    printf("  ");
  }
}

void printTree(Pager* pager, uint32_t page_num, uint32_t indentation_level) {
  auto node = pager->getPage(page_num);
  uint32_t num_keys, child;

    if(NodeLeaf == node->type) {
        num_keys = leafNodeNumCells(node);
        indent(indentation_level);
        printf("- leaf (size %d)\n", num_keys);
        for (uint32_t i = 0; i < num_keys; i++) {
            indent(indentation_level + 1);
            printf("- %d\n", leafNodeCellKey(node, i));
        }
    }
    else {
        num_keys = *internalNodeNumKeys(node);
        indent(indentation_level);
        printf("- internal (size %d)\n", num_keys);
        for (uint32_t i = 0; i < num_keys; i++) {
            child = *internalNodeChild(node, i);
            printTree(pager, child, indentation_level + 1);

            indent(indentation_level + 1);
            printf("- key %d\n", *internalNodeCellKey(node, i));
        }
        child = *internalNodeRightChild(node);
        printTree(pager, child, indentation_level + 1);
    }
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
    if (pagesCount == 0) {
        // Initialise root node
        auto root = getPage(0);
        initLeafNode(root);
        root->isRoot = true;
    }
}

Node* Pager::getPage(const uint32_t pageNum) {
    if (pageNum > MAX_PAGES) {
        throw std::runtime_error("Tried to get page out of bounds");
    }

    if (pages[pageNum] == nullptr){
        // Cache miss
        pages[pageNum] = new Node();
        auto numPages = fileLength / PAGE_SIZE;

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

void copyString(char* dest, const std::string& src, const uint32_t limit){
    if (src.length() > limit) {
        throw std::runtime_error("arguments too large to be inserted");
    }
    for (size_t i = 0; i < limit; i++){
        dest[i] = src[i];
    }
}

void Table::insert(const std::vector<std::string> &args) {
    auto node = getPage(rootPageNum);

    Row row = Row();
    row.id = std::stoi(args[1]);
    copyString(row.username, args[2], USER_LEN);
    copyString(row.email, args[3], EMAIL_LEN);

    auto cursor = Cursor(this);
    cursor.tableFind(row.id);
    if (cursor.getCellNum() < leafNodeNumCells(node)){
        auto keyAtIndex = leafNodeCellKey(node, cursor.getCellNum());
        if (*keyAtIndex == row.id) {
            std::cout << "Duplicate key found, no insertion into db\n";
            return;
        }
    }
    cursor.leafNodeInsert(row.id, row);
}

void printRow(const Row* row) {
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

void Table::select(){
    auto row = Row();
    auto cursor = Cursor(this);
    while (!(cursor.atEndOfTable())) {
        auto slot = cursor.value();
        std::memcpy(&row, slot, LEAF_NODE_VALUE_SIZE);
        printRow(&row);
        cursor.advance();
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

void Cursor::leafNodeInsert(uint32_t key, const Row& value) {
    auto node = table->getPage(pageNum);
    auto numCells = leafNodeNumCells(node);
    if (numCells >= LEAF_NODE_MAX_CELLS){
        leafNodeSplitInsert(key, value);
        return;
    }

    if (cellNum < numCells) {
        for (auto i = numCells; i > cellNum; i--){
            memcpy(leafNodeCell(node, i), leafNodeCell(node, i-1), LEAF_NODE_CELL_SIZE);
        }
    }
    incrementNumCells(node);
    setKey(node, cellNum, key);
    memcpy(leafNodeCellValue(node, cellNum), &value, sizeof(Row));
}
void Cursor::leafNodeFind(uint32_t p, uint32_t key) {
    auto node = table->getPage(p);
    auto numCells = leafNodeNumCells(node);
    this->pageNum = p;
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

void Cursor::leafNodeSplitInsert(uint32_t key, const Row &value) {
    auto oldNode = table->getPage(pageNum);
    auto newPageNum = table->getUnusedPageNum();
    auto newNode = table->getPage(newPageNum);
    initLeafNode(newNode);

    for (auto i = (int32_t)LEAF_NODE_MAX_CELLS; i >= 0; i--){
        auto destinationNode = (i >= LEAF_NODE_LEFT_SPLIT_COUNT) ? newNode : oldNode;
        auto indexWithinNode = i % LEAF_NODE_LEFT_SPLIT_COUNT;
        auto destinationCell = leafNodeCell(destinationNode, indexWithinNode);

        if (i == cellNum) {
            memcpy(destinationCell + LEAF_NODE_KEY_SIZE, &value, LEAF_NODE_VALUE_SIZE);
        } else if (i > cellNum) {
            memcpy(destinationCell, leafNodeCell(oldNode, i-1),  LEAF_NODE_CELL_SIZE);
        } else {
            memcpy(destinationCell, leafNodeCell(oldNode, i), LEAF_NODE_CELL_SIZE);
        }
    }
    setNumCells(oldNode, LEAF_NODE_LEFT_SPLIT_COUNT);
    setNumCells(newNode, LEAF_NODE_RIGHT_SPLIT_COUNT);

    if (oldNode->isRoot) {
        table->createNewRoot(newPageNum);
    } else {
        printf("todo\n");
    }
}

void Table::createNewRoot(uint32_t rightChildPageNum) {
    auto root = getPage(rootPageNum);
    auto rightChild = getPage(rightChildPageNum);
    auto leftChildPageNum = getUnusedPageNum();
    auto leftChild = getPage(leftChildPageNum);

    memcpy(leftChild, root, PAGE_SIZE);
    leftChild->isRoot = false;

    initInternalNode(root);
    root->isRoot = true;
    *internalNodeNumKeys(root) = 1;
    *internalNodeChild(root, 0) = leftChildPageNum;
    auto leftChildMaxKey = getNodeMaxKey(leftChild);
    *internalNodeCellKey(root, 0) = leftChildMaxKey;
    *internalNodeRightChild(root) = rightChildPageNum;
}

