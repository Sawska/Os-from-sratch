#ifndef USER_SYSCALL_H
#define USER_SYSCALL_H

#include "syscall.h"
#include <vector>
#include <string>
#include <cstring>

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



#endif // USER_SYSCALL_H
