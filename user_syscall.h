#ifndef USER_SYSCALL_H
#define USER_SYSCALL_H

#include "syscall.h"
#include <vector>
#include <string>
#include <cstring>
#include "PCB.h"
#include "memory_protection.cpp"
#include "Filesystem.h"
#include <string>
#include <iostream>
#include "Heap.h"

inline int sys_open(const char* filename) {
    int fd;
    asm volatile (
        "mov $0x80, %%eax;"
        "mov %1, %%ebx;"
        "int $0x80;"
        "mov %%eax, %0;"
        : "=r" (fd)
        : "r" (filename)
        : "%eax", "%ebx"
    );
    return fd;
}

inline int sys_close(int fd) {
    int ret;
    asm volatile (
        "mov $0x81, %%eax;"
        "mov %1, %%ebx;"
        "int $0x80;"
        "mov %%eax, %0;"
        : "=r" (ret)
        : "r" (fd)
        : "%eax", "%ebx"
    );
    return ret;
}

inline int sys_read(int fd, void* buffer, int count) {
    int ret;
    asm volatile (
        "mov $0x82, %%eax;"
        "mov %1, %%ebx;"
        "mov %2, %%ecx;"
        "mov %3, %%edx;"
        "int $0x80;"
        "mov %%eax, %0;"
        : "=r" (ret)
        : "r" (fd), "r" (buffer), "r" (count)
        : "%eax", "%ebx", "%ecx", "%edx"
    );
    return ret;
}

inline int sys_write(int fd, const void* buffer, int count) {
    int ret;
    asm volatile (
        "mov $0x83, %%eax;"
        "mov %1, %%ebx;"
        "mov %2, %%ecx;"
        "mov %3, %%edx;"
        "int $0x80;"
        "mov %%eax, %0;"
        : "=r" (ret)
        : "r" (fd), "r" (buffer), "r" (count)
        : "%eax", "%ebx", "%ecx", "%edx"
    );
    return ret;
}

inline int sys_lseek(int fd, int offset, int whence) {
    int ret;
    asm volatile (
        "mov $0x84, %%eax;"
        "mov %1, %%ebx;"
        "mov %2, %%ecx;"
        "mov %3, %%edx;"
        "int $0x80;"
        "mov %%eax, %0;"
        : "=r" (ret)
        : "r" (fd), "r" (offset), "r" (whence)
        : "%eax", "%ebx", "%ecx", "%edx"
    );
    return ret;
}

inline void sys_exit(int status) {
    asm volatile (
        "mov $0x85, %%eax;"
        "mov %0, %%ebx;"
        "int $0x80;"
        : 
        : "r" (status)
        : "%eax", "%ebx"
    );
}

inline int sys_mkdir(const char* path) {
    int ret;
    asm volatile (
        "mov $0x86, %%eax;"   
        "mov %1, %%ebx;"      
        "int $0x80;"          
        "mov %%eax, %0;"      
        : "=r" (ret)
        : "r" (path)
        : "%eax", "%ebx"
    );
    return ret;
}

inline int sys_unlink(const char* filename) {
    int ret;
    asm volatile (
        "mov $0x82, %%eax;"       
        "mov %1, %%ebx;"          
        "int $0x80;"
        "mov %%eax, %0;"
        : "=r" (ret)
        : "r" (filename)
        : "%eax", "%ebx"
    );
    return ret;
}


inline std::vector<std::string> sys_ls() {
    char buffer[1024];  
    int ret;

    
    asm volatile (
        "mov $0x83, %%eax;"       
        "mov %1, %%ebx;"          
        "mov %2, %%ecx;"          
        "int $0x80;"
        "mov %%eax, %0;"
        : "=r" (ret)
        : "r" (buffer), "r" (sizeof(buffer))
        : "%eax", "%ebx", "%ecx"
    );

    std::vector<std::string> contents;
    if (ret >= 0) {
        const char* start = buffer;
        while (start < buffer + ret) {
            std::string entry(start);
            contents.push_back(entry);
            start += entry.length() + 1;
        }
    }
    return contents;
}



int sys_fork() {
    PCB* parentPCB = getCurrentProcess();
    PCB* childPCB = new PCB(*parentPCB); 

    childPCB->processID = generateNewPID();
    childPCB->parentProcessID = parentPCB->processID;

    addProcessToScheduler(childPCB);

    return childPCB->processID;
}


int sys_exec(const char* path) {
    PCB* process = getCurrentProcess();
    clearProcessMemory(process);

    if (!loadProgramIntoMemory(path, process->memory)) {
        return -1; 
    }
    process->entryPoint = process->memory->getEntryPoint();
    setProgramCounter(process->entryPoint);
    
    return 0;
}
void sys_exit(int status) {
    PCB* process = getCurrentProcess();
    process->status = status;
    freeProcessResources(process);
    removeProcessFromScheduler(process);
}


void* sys_malloc(size_t size) {
    return malloc(size);  
}

void sys_free(void* ptr) {
    free(ptr);  
}




std::string currentDirectory = "/";

int sys_cd(const char* path) {
    if (changeDirectory(path)) {
        currentDirectory = path;
        return 0;
    } else {
        return -1; 
    }
}

std::string sys_pwd() {
    return currentDirectory;
}



int sys_cat(const char* filename) {
    int fd = open(filename);
    if (fd < 0) return -1;

    char buffer[1024];
    int bytesRead;
    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        std::cout.write(buffer, bytesRead);
    }
    close(fd);
    return 0;
}



#endif // USER_SYSCALL_H
