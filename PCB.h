#ifndef PCB_H
#define PCB_H

#define MAX_REGISTERS 8

enum ProcessState {NEW, READY,RUNNING,WAITING,TERMINATED};

struct PCB {
    int processID;
    ProcessState state;
    int registers[MAX_REGISTERS];
    int stackPointer;
    int programCounter;
    int priority;
    int* pageTable;
};

#endif //PCB_H