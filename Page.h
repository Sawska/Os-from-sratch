#ifndef PAGE_H
#define PAGE_H

#include <stdint.h>
#include "replacment_algos.cpp"

#define PAGE_DIRECTORY_SIZE 1024
#define PAGE_TABLE_SIZE 1024
#define PAGE_SIZE 4096
#define PAGE_SIZE 4096
#define TOTAL_PAGES 1024  

uint32_t page_directory[PAGE_DIRECTORY_SIZE] __attribute__((aligned(4096)));
uint32_t first_page_table[PAGE_TABLE_SIZE] __attribute__((aligned(4096)));

static uint8_t pageBitmap[TOTAL_PAGES] = {0};

void initializePaging() {
    for(int i =0;i<PAGE_TABLE_SIZE;i++) {
        first_page_table[i] = (i * PAGE_SIZE) | 3;
    }

    page_directory[0] = ((uint32_t)first_page_table) | 3;

    for(int i =1;i<PAGE_DIRECTORY_SIZE;i++) {
        page_directory[i] = 0;
    }

    __asm__ volatile("mov %0, %%cr3" :: "r"(&page_directory));

    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile("mov %0, %%cr0" :: "r"(cr0));
}

void* allocatePage() {
    for (int i = 0; i < TOTAL_PAGES; i++) {
        if (pageBitmap[i] == 0) {  
            pageBitmap[i] = 1;     
            return (void*)(i * PAGE_SIZE);
        }
    }
    return 0;
}

void freePage(void* page) {
    int pageIndex = (uint32_t)page / PAGE_SIZE;
    if (pageIndex < TOTAL_PAGES) {
        pageBitmap[pageIndex] = 0;
    }
}

void freePage(void* page) {
    int pageIndex = (uint32_t)page / PAGE_SIZE;
    if (pageIndex < TOTAL_PAGES) {
        pageBitmap[pageIndex] = 0; 
    }
}

void setupProcessStack(uint32_t* page_directory, uint32_t virtual_address, uint32_t physical_address) {
    int page_dir_index = virtual_address >> 22; 
    int page_table_index = (virtual_address >> 12) & 0x03FF;

    
    if (!(page_directory[page_dir_index] & 1)) { 
    
        uint32_t* new_page_table = (uint32_t*)allocatePage();
        for (int i = 0; i < 1024; i++) new_page_table[i] = 0; 

        page_directory[page_dir_index] = ((uint32_t)new_page_table) | 3; 
    }

    
    uint32_t* page_table = (uint32_t*)(page_directory[page_dir_index] & 0xFFFFF000);
    page_table[page_table_index] = physical_address | 3;  
}

void handlePageFault(int page, uint32_t virtual_address, uint32_t physical_address) {
    switch (currentAlgorithm) {
        case FIFO:
            handlePageFault_FIFO(page);
            break;
        case LRU:
            handlePageFault_LRU(page);
            break;
        case CLOCK:
            handlePageFault_Clock(page);
            break;
    }


    setupProcessStack(page_directory, virtual_address, physical_address);
}

extern "C" void pageFaultHandler(uint32_t error_code, uint32_t virtual_address) {
    int page_to_load = virtual_address / PAGE_SIZE;
    if (!(pageBitmap[page_to_load] & 1)) { 
        void* physical_page = allocatePage();
        if (physical_page == nullptr) {
            printf("Out of memory\n");
            return;
        }
        
        uint32_t physical_address = (uint32_t)physical_page;

        handlePageFault(page_to_load, virtual_address, physical_address);
    } else {
        
    }
}

enum ReplacementAlgorithm { FIFO, LRU, CLOCK };
ReplacementAlgorithm currentAlgorithm = FIFO;


#endif //PAGE_H
