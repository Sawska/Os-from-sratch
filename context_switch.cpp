
#include "pcb.h"
void contextSwitch(PCB* currentProcess, PCB* nextProcess) {
    saveProcessState(currentProcess);
    restoreProcessState(nextProcess);

}
