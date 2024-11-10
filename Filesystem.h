#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdint.h>

#define MAX_FILES 128     
#define BLOCK_SIZE 512    
#define MAX_BLOCKS 4096   

struct Superblock {
    uint32_t totalBlocks; 
    uint32_t freeBlocks;  
    uint32_t rootDirectory;
};

uint8_t disk[MAX_BLOCKS][BLOCK_SIZE];
struct DirectoryEntry {
    char filename[32];
    uint32_t size;         
    uint32_t startBlock;   
    uint32_t isDirectory;  
};

struct FileAllocationTable {
    uint32_t blocks[MAX_BLOCKS];
};
Superblock superblock;
DirectoryEntry directory[MAX_FILES];
FileAllocationTable fat;

void formatDisk() {
    Superblock superblock = {MAX_BLOCKS, MAX_BLOCKS - 1, 1};
    FileAllocationTable fat = {0};  

    DirectoryEntry root = {"root", 0, 1, 1};


    writeBlock(0, (uint8_t*)&superblock);
    writeBlock(1, (uint8_t*)&root);      
    writeBlock(2, (uint8_t*)&fat);       
}

bool createFile(const char* filename) {
    
    if (findFile(filename)) return false;

    
    int dirIndex = findFreeDirectoryEntry();
    if (dirIndex < 0) return false; 

    
    int startBlock = findFreeBlock();
    if (startBlock < 0) return false;

    
    DirectoryEntry newFile;
    strncpy(newFile.filename, filename, 32);
    newFile.size = 0;
    newFile.startBlock = startBlock;
    newFile.isDirectory = 0;

    directory[dirIndex] = newFile;

    
    fat.blocks[startBlock] = 0xFFFFFFFF;
    return true;
}

int readFile(const char* filename, uint8_t* buffer, int maxSize) {
    int dirIndex = findFile(filename);
    if (dirIndex < 0) return -1;

    DirectoryEntry file = directory[dirIndex];
    int bytesRead = 0;
    int currentBlock = file.startBlock;

    while (currentBlock != 0xFFFFFFFF && bytesRead < maxSize) {
        readBlock(currentBlock, buffer + bytesRead);
        bytesRead += BLOCK_SIZE;
        currentBlock = fat.blocks[currentBlock]; 
    }
    return bytesRead;
}


#define MAX_OPEN_FILES 10

struct FileDescriptor {
    DirectoryEntry* entry;
    int position; 
};

FileDescriptor openFiles[MAX_OPEN_FILES];

int sys_open(const char* path) {
    int fd = open(path);
    if (fd < 0) return -1; 
    return fd;
}

int sys_read(int fd, void* buffer, int size) {
    return readFileDescriptor(fd, buffer, size); 
}

#include <string.h>
#include <stdbool.h>

#define MAX_OPEN_FILES 10
#define FILENAME_MAX_LEN 32
#define END_OF_FILE 0xFFFFFFFF





int findFile(const char* filename);
int findFreeDirectoryEntry();
int findFreeBlock();
void readBlock(int blockNum, uint8_t* buffer);
void writeBlock(int blockNum, uint8_t* buffer);

int open(const char* path) {
    int dirIndex = findFile(path);
    if (dirIndex < 0) return -1;  

    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (openFiles[i].entry == nullptr) {
            openFiles[i].entry = &directory[dirIndex];
            openFiles[i].position = 0;
            return i;
        }
    }
    return -1;  
}

bool changeDirectory(const char* path) {
    int dirIndex = findFile(path); 
    if (dirIndex < 0) return false;

    DirectoryEntry* entry = &directory[dirIndex];
    if (entry->isDirectory) {
        return true;
    }
    return false;  
}



void close(int fd) {
    if (fd >= 0 && fd < MAX_OPEN_FILES && openFiles[fd].entry != nullptr) {
        openFiles[fd].entry = nullptr;
    }
}


int read(int fd, void* buffer, int size) {
    if (fd < 0 || fd >= MAX_OPEN_FILES || openFiles[fd].entry == nullptr) return -1;

    FileDescriptor* fileDesc = &openFiles[fd];
    DirectoryEntry* file = fileDesc->entry;
    int bytesRead = 0;
    int currentBlock = file->startBlock;
    int position = fileDesc->position;

    while (currentBlock != END_OF_FILE && bytesRead < size) {
        uint8_t tempBuffer[BLOCK_SIZE];
        readBlock(currentBlock, tempBuffer);

        int toRead = size - bytesRead < BLOCK_SIZE ? size - bytesRead : BLOCK_SIZE;
        memcpy((uint8_t*)buffer + bytesRead, tempBuffer + position, toRead);

        bytesRead += toRead;
        position = 0; 
        currentBlock = fat.blocks[currentBlock];
    }

    fileDesc->position += bytesRead;
    return bytesRead;
}

int write(int fd, const void* buffer, int size) {
    if (fd < 0 || fd >= MAX_OPEN_FILES || openFiles[fd].entry == nullptr) return -1;

    FileDescriptor* fileDesc = &openFiles[fd];
    DirectoryEntry* file = fileDesc->entry;
    int bytesWritten = 0;
    int currentBlock = file->startBlock;
    int position = fileDesc->position;

    while (bytesWritten < size) {
        if (currentBlock == END_OF_FILE) {
            currentBlock = findFreeBlock();
            if (currentBlock < 0) return -1;
            fat.blocks[file->startBlock] = currentBlock;
            file->startBlock = currentBlock;
        }

        uint8_t tempBuffer[BLOCK_SIZE];
        readBlock(currentBlock, tempBuffer);

        int toWrite = size - bytesWritten < BLOCK_SIZE ? size - bytesWritten : BLOCK_SIZE;
        memcpy(tempBuffer + position, (uint8_t*)buffer + bytesWritten, toWrite);
        writeBlock(currentBlock, tempBuffer);

        bytesWritten += toWrite;
        position = 0;
        currentBlock = fat.blocks[currentBlock];
    }

    fileDesc->position += bytesWritten;
    return bytesWritten;
}

bool mkdir(const char* name) {
    if (findFile(name) >= 0) return false; 

    int dirIndex = findFreeDirectoryEntry();
    int startBlock = findFreeBlock();
    if (dirIndex < 0 || startBlock < 0) return false;

    DirectoryEntry newDir;
    strncpy(newDir.filename, name, FILENAME_MAX_LEN);
    newDir.size = 0;
    newDir.startBlock = startBlock;
    newDir.isDirectory = 1;

    directory[dirIndex] = newDir;
    fat.blocks[startBlock] = END_OF_FILE;

    return true;
}


bool unlink(const char* path) {
    int dirIndex = findFile(path);
    if (dirIndex < 0) return false;

    DirectoryEntry* entry = &directory[dirIndex];
    int currentBlock = entry->startBlock;

    while (currentBlock != END_OF_FILE) {
        int nextBlock = fat.blocks[currentBlock];
        fat.blocks[currentBlock] = 0; 
        currentBlock = nextBlock;
    }
    memset(entry, 0, sizeof(DirectoryEntry));
    return true;
}

void updateFileSystemState() {
    writeBlock(0, (uint8_t*)&superblock);
    writeBlock(1, (uint8_t*)directory);
    writeBlock(2, (uint8_t*)&fat);
}


int findFile(const char* filename) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (strcmp(directory[i].filename, filename) == 0 && directory[i].filename[0] != '\0') {
            return i;
        }
    }
    return -1;
}

int findFreeDirectoryEntry() {
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].filename[0] == '\0') { 
            return i;
        }
    }
    return -1;
}


int findFreeBlock() {
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (fat.blocks[i] == 0) { 
            return i;
        }
    }
    return -1;
}

void readBlock(int blockNum, uint8_t* buffer) {
    if (blockNum >= 0 && blockNum < MAX_BLOCKS) {
        memcpy(buffer, disk[blockNum], BLOCK_SIZE); 
    }
}


void writeBlock(int blockNum, uint8_t* buffer) {
    if (blockNum >= 0 && blockNum < MAX_BLOCKS) {
        memcpy(disk[blockNum], buffer, BLOCK_SIZE); 
    }
}

int readFileDescriptor(int fd, void* buffer, int size) {
    if (fd < 0 || fd >= MAX_OPEN_FILES || openFiles[fd].entry == nullptr) {
        return -1; 
    }
    return read(fd, buffer, size);
}


#endif //FILESYSTEM_H