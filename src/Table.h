//
// Created by potato on 06/06/2020.
//

#ifndef DB_ENGINE_TABLE_H
#define DB_ENGINE_TABLE_H

#include <stdint.h>
#include <vector>
#include <string>
#include <array>
#include <cstring>

// Database schema with number of bytes per column
// id - integer(4)
// username - varChar(32)
// email - varchar(255)
const uint32_t USER_LEN = 32;
const uint32_t EMAIL_LEN = 255;
typedef struct Row{
    uint32_t id;
    char username[USER_LEN];
    char email[EMAIL_LEN];
    Row() = default;
}Row;

static const uint32_t PAGE_SIZE = 4096;
static const uint32_t MAX_PAGES = 100;

typedef enum: uint8_t {NodeInternal, NodeLeaf} NodeType;

/*
 * Common Node Header Layout
 */
const uint32_t NODE_TYPE_SIZE = sizeof(uint8_t);
const uint32_t NODE_TYPE_OFFSET = 0;
const uint32_t IS_ROOT_SIZE = sizeof(uint8_t);
const uint32_t IS_ROOT_OFFSET = NODE_TYPE_SIZE;
const uint32_t PARENT_POINTER_SIZE = sizeof(void *);
const uint32_t PARENT_POINTER_OFFSET = IS_ROOT_OFFSET + IS_ROOT_SIZE;
const uint8_t COMMON_NODE_HEADER_SIZE = NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE;


/*
 * Leaf Node Header Layout
 */
const uint32_t LEAF_NODE_NUM_CELLS_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_NUM_CELLS_OFFSET = COMMON_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUM_CELLS_SIZE;

/*
 * Leaf Node Body Layout
 */
const uint32_t LEAF_NODE_KEY_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_KEY_OFFSET = 0;
const uint32_t LEAF_NODE_VALUE_SIZE = sizeof(Row);
const uint32_t LEAF_NODE_VALUE_OFFSET = LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE;
const uint32_t LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;
const uint32_t LEAF_NODE_SPACE_FOR_CELLS = PAGE_SIZE - LEAF_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_MAX_CELLS = LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;

void printConstants();
const uint32_t LEAF_NODE_RIGHT_SPLIT_COUNT = (LEAF_NODE_MAX_CELLS + 1) / 2;
const uint32_t LEAF_NODE_LEFT_SPLIT_COUNT =
    (LEAF_NODE_MAX_CELLS + 1) - LEAF_NODE_RIGHT_SPLIT_COUNT;

typedef struct Node {
    NodeType type;
    uint8_t isRoot;
    Node *parent;
    uint8_t data[PAGE_SIZE - COMMON_NODE_HEADER_SIZE];
    Node(): parent(nullptr), type(NodeLeaf), isRoot(false){};
} Node;

/*
 * Internal Node Header Layout
 */
const uint32_t INTERNAL_NODE_NUM_KEYS_SIZE = sizeof(uint32_t);
const uint32_t INTERNAL_NODE_NUM_KEYS_OFFSET = COMMON_NODE_HEADER_SIZE;
const uint32_t INTERNAL_NODE_RIGHT_CHILD_SIZE = sizeof(uint32_t);
const uint32_t INTERNAL_NODE_RIGHT_CHILD_OFFSET =
    INTERNAL_NODE_NUM_KEYS_OFFSET + INTERNAL_NODE_NUM_KEYS_SIZE;
const uint32_t INTERNAL_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE +
                                           INTERNAL_NODE_NUM_KEYS_SIZE +
                                           INTERNAL_NODE_RIGHT_CHILD_SIZE;
/*
 * Internal Node Body Layout
 */
const uint32_t INTERNAL_NODE_KEY_SIZE = sizeof(uint32_t);
const uint32_t INTERNAL_NODE_CHILD_SIZE = sizeof(uint32_t);
const uint32_t INTERNAL_NODE_CELL_SIZE =
    INTERNAL_NODE_CHILD_SIZE + INTERNAL_NODE_KEY_SIZE;

uint32_t leafNodeNumCells(Node* node);
uint8_t* leafNodeCell(Node* node, uint32_t cellNum);
uint32_t* leafNodeCellKey(Node*  node, uint32_t cellNum);
uint8_t* leafNodeCellValue(Node* node, uint32_t cellNum);
void incrementNumCells(Node* node);
void setKey(Node* node, uint32_t cellNum, uint32_t key);

class Pager{
private:
    uint32_t pagesCount = 0;
public:
    Pager() = default;
    ~Pager() {
        for (auto page: pages){
            delete page;
        }
    }
    void pagerOpen(const std::string& fileName);
    Node* getPage(uint32_t pageNum);
    void flushPage(uint32_t pageIndex);

    [[nodiscard]] uint32_t getPagesCount() const {return pagesCount;}
    uint32_t getUnusedPageNum() {return pagesCount;}

    int fileDescriptor;
    uint32_t fileLength;
    std::array<Node*, MAX_PAGES> pages;
};

// Data will be stored in pages
class Table {
private:
    Pager* pager;
    uint32_t rootPageNum = 0;
public:
    Table() {
        pager = new Pager;
        pager->pagerOpen("db_out");
        rootPageNum = 0;
    }
    explicit Table(const std::string& fileName) {
        pager = new Pager;
        pager->pagerOpen(fileName);
        rootPageNum = 0;
    }
    ~Table() {
        delete pager;
    }
    void insert(const std::vector<std::string>& args);
    void select();
    void createNewRoot(uint32_t rootPageNum);

    void tableClose();
    Node* getPage(const uint32_t pageNum) {return pager->getPage(pageNum);}
    [[nodiscard]] uint32_t getRootPageNum() const {return rootPageNum;}
    uint32_t getUnusedPageNum() {return pager->getUnusedPageNum();}
    Pager* getPager() {return pager;}
};

class Cursor {
private:
    Table *table;
    uint32_t pageNum;
    uint32_t cellNum;
    bool endOfTable = true;
public:
    explicit Cursor(Table *table): table(table) {
        pageNum = table->getRootPageNum();
        cellNum = 0;
        auto rootNode = table->getPage(table->getRootPageNum());
        auto numCells = leafNodeNumCells(rootNode);
        endOfTable = (numCells == 0);
    }
    ~Cursor() {}
    uint8_t* value();
    void advance();
    void tableFind(uint32_t key);
    void leafNodeFind(uint32_t rootPageNum, uint32_t key);
    void leafNodeInsert(uint32_t key, const Row& value);
    void leafNodeSplitInsert(uint32_t key, const Row& value);
    void internalNodeFind(uint32_t rootPageNum, uint32_t key);

    [[nodiscard]] uint32_t getCellNum() const {return cellNum;}
    [[nodiscard]] bool atEndOfTable() const {return endOfTable;}
    [[nodiscard]] uint32_t getPageNum() const {return pageNum;}
    void setPageNum(uint32_t p) {pageNum = p;}
    void setCellNum(uint32_t x) {cellNum = x;}

};
void printTree(Pager* pager, uint32_t page_num, uint32_t indentation_level);
#endif //DB_ENGINE_TABLE_H
