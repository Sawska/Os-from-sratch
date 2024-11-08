#include "syscall.h"
#include "filesystem.h"
#include "pcb.h"

extern "C" void syscallHandler(int syscallNumber, int arg1, int arg2, int arg3) {
    switch (syscallNumber) {
        case SYSCALL_OPEN:
            int fd = open(reinterpret_cast<const char*>(arg1));
            break;
        case SYSCALL_CLOSE:
            close(arg1);
            break;
        case SYSCALL_READ:
            
            read(arg1, reinterpret_cast<void*>(arg2), arg3);
            break;
        case SYSCALL_WRITE:
            write(arg1, reinterpret_cast<const void*>(arg2), arg3);
            break;
        case SYSCALL_MKDIR:
            mkdir(reinterpret_cast<const char*>(arg1));
            break;
        case SYSCALL_UNLINK:
            unlink(reinterpret_cast<const char*>(arg1));
            break;
        case SYSCALL_EXIT:
            terminateProcess(findProcess(arg1));
            break;
        case SYSCALL_CREATE_PROCESS:
            createProcess(arg1);
            break;
        default:
            
            break;
    }
}
