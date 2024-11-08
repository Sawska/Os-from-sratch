// kernel.cpp
#include "filesystem.h"
#include "pcb.h"
#include "journal.h"
#include "shell.cpp"

int main() {
    
    initializePaging();
    formatDisk();
    recoverFromJournal();
    
    
    PCB* shellProcess = createProcess(reinterpret_cast<int>(&shell));
    
    
    readyQueue.push(shellProcess);
    
    
    schedule();
    
    return 0;
}
