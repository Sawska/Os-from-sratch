#ifndef USER_SYSCALL_H
#define USER_SYSCALL_H

#include "syscall.h"
#include <vector>
#include <string>
#include <cstring>
#include "PCB.h"
#include "memory_protection.cpp"
#include "Filesystem.h"

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
