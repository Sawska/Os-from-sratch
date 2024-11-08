#include "Page.h"
#include "PCB.h"


PageTable* allocatePageDirectory() {
    PageTable* pageTable = new PageTable(256);
    return pageTable;
}


void setupMemoryProtection(PCB* process) {
    
    process->pageTable = allocatePageDirectory();
    
}
