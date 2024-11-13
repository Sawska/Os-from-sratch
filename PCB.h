#ifndef PCB_H
#define PCB_H

#include <iostream>
#include <vector>
#include <queue>
#include "Page.h"



#define MAX_REGISTERS 8
#define SYSCALL_CREATE 1
#define SYSCALL_EXIT 2
#define SYSCALL_YIELD 3

enum ProcessState { NEW, READY, RUNNING, WAITING, TERMINATED };

struct Memory {
    char* data;  
    size_t size;

    Memory(size_t s) : size(s) {
        data = new char[size];
    }

    ~Memory() {
        delete[] data;
    }

    int getEntryPoint() const {
        return 0;
    }
};



struct PageTableEntry {
    int frameNumber;  
    bool present;     
    bool readOnly;    
};

class PageTable {
public:
    std::vector<PageTableEntry> entries;
    PageTable(size_t size) : entries(size) {}

  
};

struct PCB {
    int processID;
    int parentProcessID;  
    ProcessState state;
    int registers[MAX_REGISTERS];
    int stackPointer;
    int programCounter;
    int priority;
    PageTable* pageTable;
    int status;
    Memory* memory;      
    int entryPoint;
};




std::queue<PCB*> readyQueue;
std::vector<PCB*> processTable;

#define STACK_SIZE (PAGE_SIZE * 4) 
#define STACK_BASE_VIRTUAL_ADDRESS 0xBFFF0000  

int initializeStack() {
    uint32_t virtual_address = STACK_BASE_VIRTUAL_ADDRESS;

    for (int i = 0; i < STACK_SIZE / PAGE_SIZE; i++) {
        uint32_t physical_address = (uint32_t)allocatePage();
        setupProcessStack(page_directory, virtual_address, physical_address);

        virtual_address += PAGE_SIZE;  
    }

    return STACK_BASE_VIRTUAL_ADDRESS + STACK_SIZE - 4;
}

PCB* currentProcess = nullptr;

PCB* getCurrentProcess() {
    return currentProcess;
}

PCB* findProcess(int processID) {
    for (auto* process : processTable) {
        if (process->processID == processID) {
            return process;
        }
    }
    return nullptr;
}

int nextPID = 1;  

int generateNewPID() {
    return nextPID++;
}


void runProcess(PCB* process) {
    process->state = RUNNING;

    process->programCounter += 4;
}

void saveProcessState(PCB* process) {


    for (int i = 0; i < MAX_REGISTERS; i++) {
        process->registers[i] = process->programCounter + i;
    }
    process->stackPointer = process->programCounter + 100;
}

void restoreProcessState(PCB* process) {

    process->programCounter = process->stackPointer - 100;
}

PCB* createProcess(int entryPoint) {
    static int nextProcessID = 1;
    PCB* newProcess = new PCB();

    newProcess->processID = nextProcessID++;
    newProcess->programCounter = entryPoint;
    newProcess->state = NEW;
    newProcess->stackPointer = initializeStack();
    newProcess->pageTable = nullptr;
    newProcess->priority = 0;

    processTable.push_back(newProcess);  
    readyQueue.push(newProcess);         
    newProcess->state = READY;

    return newProcess;
}

void terminateProcess(PCB* process) {
    process->state = TERMINATED;

    auto it = std::remove(processTable.begin(), processTable.end(), process);
    if (it != processTable.end()) {
        processTable.erase(it);
    }

    delete[] process->pageTable;  
    delete process;               
}

void contextSwitch(PCB* currentProcess, PCB* nextProcess) {
    saveProcessState(currentProcess);
    restoreProcessState(nextProcess);
}

void handleSyscall(int syscallID, PCB* process) {
    switch (syscallID) {
        case SYSCALL_CREATE:
            createProcess(process->programCounter);
            break;
        case SYSCALL_EXIT:
            terminateProcess(process);
            break;
        case SYSCALL_YIELD:
            process->state = READY;  
            readyQueue.push(process);
            schedule();              
            break;
        default:
            std::cerr << "Unknown syscall ID: " << syscallID << std::endl;
            break;
    }
}

void schedule() {
    while (!readyQueue.empty()) {
        PCB* currentProcess = readyQueue.front();
        readyQueue.pop();

        currentProcess->state = RUNNING;
        runProcess(currentProcess);

        if (currentProcess->state != TERMINATED) {
            currentProcess->state = READY;
            readyQueue.push(currentProcess);
        }
    }
}

void addProcessToScheduler(PCB* process) {
    process->state = READY; 
    readyQueue.push(process);
}

void freeProcessResources(PCB* process) {
    if (process->pageTable) {
        delete process->pageTable;
        process->pageTable = nullptr;
    }
}

void removeProcessFromScheduler(PCB* process) {
    std::queue<PCB*> tempQueue;
    while (!readyQueue.empty()) {
        PCB* p = readyQueue.front();
        readyQueue.pop();
        if (p != process) {
            tempQueue.push(p);
        }
    }
    std::swap(readyQueue, tempQueue);

    auto it = std::remove(processTable.begin(), processTable.end(), process);
    if (it != processTable.end()) {
        processTable.erase(it);
    }
}

void clearProcessMemory(PCB* process) {
    if (process->memory) {
        delete process->memory;
        process->memory = nullptr;
    }
}

bool loadProgramIntoMemory(const char* path, Memory*& memory) {
    memory = new Memory(1024);  
    std::fill(memory->data, memory->data + memory->size, 0);  
    return true;  
}
void setProgramCounter(int entryPoint) {
    PCB* process = getCurrentProcess();
    process->programCounter = entryPoint;
}




#endif // PCB_H

