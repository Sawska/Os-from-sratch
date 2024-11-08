#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "user_syscall.h"

void shell() {
    std::string input;
    while (true) {
        std::cout << "shell> ";
        std::getline(std::cin, input);

        std::istringstream iss(input);
        std::string cmd;
        iss >> cmd;

        if (cmd == "exit") {
            break;
        } else if (cmd == "mkdir") {
            std::string dirname;
            iss >> dirname;
            if (sys_mkdir(dirname.c_str())) {
                std::cout << "Directory created successfully.\n";
            } else {
                std::cout << "Failed to create directory.\n";
            }
        } else if (cmd == "ls") {
            std::vector<std::string> contents = sys_ls();
            for (const std::string& item : contents) {
                std::cout << item << "\n";
            }
        } else if (cmd == "open") {
            std::string filename;
            iss >> filename;
            int fd = sys_open(filename.c_str());
            if (fd >= 0) {
                std::cout << "File opened with descriptor: " << fd << "\n";
            } else {
                std::cout << "Failed to open file.\n";
            }
        } else if (cmd == "read") {
            int fd, size;
            iss >> fd >> size;
            char buffer[1024];  
            int bytesRead = sys_read(fd, buffer, size);
            if (bytesRead > 0) {
                std::cout << "Read " << bytesRead << " bytes: " << std::string(buffer, bytesRead) << "\n";
            } else {
                std::cout << "Failed to read file.\n";
            }
        } else if (cmd == "write") {
            int fd;
            std::string data;
            iss >> fd;
            std::getline(iss, data); 
            int bytesWritten = sys_write(fd, data.c_str(), data.size());
            if (bytesWritten > 0) {
                std::cout << "Wrote " << bytesWritten << " bytes.\n";
            } else {
                std::cout << "Failed to write to file.\n";
            }
        } else if (cmd == "close") {
            int fd;
            iss >> fd;
            if (sys_close(fd) == 0) {
                std::cout << "File descriptor closed.\n";
            } else {
                std::cout << "Failed to close file descriptor.\n";
            }
        } else if (cmd == "unlink") {
            std::string filename;
            iss >> filename;
            if (sys_unlink(filename.c_str()) == 0) {
                std::cout << "File deleted.\n";
            } else {
                std::cout << "Failed to delete file.\n";
            }
        } else {
            std::cout << "Unknown command: " << cmd << "\n";
        }
    }
}
