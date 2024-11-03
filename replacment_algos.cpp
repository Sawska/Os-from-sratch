#include <list>
#include <unordered_map>
#include <queue>
#include <unordered_set>
#include <vector>
#include "Page.h"

std::queue<int> pageQueue;           
std::unordered_set<int> pagesInMemory;

#define MAX_FRAMES 4

std::list<int> pageList;
std::unordered_map<int, std::list<int>::iterator> pageMap;

void handlePageFault_LRU(int page) {
    if (pageMap.find(page) != pageMap.end()) {
        pageList.erase(pageMap[page]);
    } else {
        if (pageList.size() == MAX_FRAMES) {
            int lruPage = pageList.back();   
            pageList.pop_back();
            pageMap.erase(lruPage);
        }
    }
    pageList.push_front(page);
    pageMap[page] = pageList.begin();
}

void handlePageFault_FIFO(int page) {
    if (pagesInMemory.size() == MAX_FRAMES) {
        int oldestPage = pageQueue.front();
        pageQueue.pop();
        pagesInMemory.erase(oldestPage);
    }
    pageQueue.push(page);
    pagesInMemory.insert(page);
}

struct PageFrame {
    int page;
    bool useBit;
};

std::vector<PageFrame> pageTable(MAX_FRAMES);
int clockPointer = 0;

void handlePageFault_Clock(int page) {
    while (true) {
        if (!pageTable[clockPointer].useBit) {
            pageTable[clockPointer].page = page;
            pageTable[clockPointer].useBit = true;
            clockPointer = (clockPointer + 1) % MAX_FRAMES;
            break;
        }

        pageTable[clockPointer].useBit = false;
        clockPointer = (clockPointer + 1) % MAX_FRAMES;
    }
}



