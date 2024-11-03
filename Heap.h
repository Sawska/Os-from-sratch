#ifndef HEAP_H

#define HEAP_H

#include <cstdio>

#define HEAP_START 0x10000
#define HEAP_SIZE  0x10000

struct Block {
    Block* next;
    bool free;
    size_t size;  
};

static Block* freeList = (Block*)HEAP_START;


void initializeHeap() {
    freeList->next = nullptr;
    freeList->free = true;
    freeList->size = HEAP_SIZE - sizeof(Block);
}

void* malloc(size_t size) {
    Block* curr = freeList;
    while (curr) {
        if (curr->free && curr->size >= size) {
            if (curr->size > size + sizeof(Block)) {  // Split the block
                Block* newBlock = (Block*)((char*)curr + sizeof(Block) + size);
                newBlock->size = curr->size - size - sizeof(Block);
                newBlock->free = true;
                newBlock->next = curr->next;
                curr->size = size;
                curr->next = newBlock;
            }
            curr->free = false;
            return (void*)((char*)curr + sizeof(Block));
        }
        curr = curr->next;
    }
    return nullptr;  
}


void free(void* ptr) {
    if (!ptr) return;
    Block* block = (Block*)((char*)ptr - sizeof(Block));
    block->free = true;


    Block* curr = freeList;

    while (curr) {
        if (curr->free && curr->next && curr->next->free) {
            curr->size += curr->next->size + sizeof(Block);
            curr->next = curr->next->next;
            curr = curr->next;
        }
    }
}


#endif //HEAP_H