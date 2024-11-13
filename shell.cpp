#include <string>
#include <sstream>
#include <vector>
#include "user_syscall.h"
#include "CDisplay.h"
#include <queue>
#include "keyboard_driver.h"

void shell() {
    std::string input;
    int y = 0;  

    while (true) {
        CDisplay::TextOut("shell> ", 0, y, BLACK, WHITE, true); 
        
        while (!shellInputBuffer.empty()) {
            char key = shellInputBuffer.front();
            shellInputBuffer.pop();
            
            if (key == '\n') { 
                if (input.empty()) continue;
                y++; 
                
                std::istringstream iss(input);
                std::string cmd;
                iss >> cmd;

                if (cmd == "exit") {
                    return;
                } else if (cmd == "mkdir") {
                    std::string dirname;
                    iss >> dirname;
                    if (sys_mkdir(dirname.c_str())) {
                        CDisplay::TextOut("Directory created successfully.", 0, y++, BLACK, WHITE);
                    } else {
                        CDisplay::TextOut("Failed to create directory.", 0, y++, BLACK, WHITE);
                    }
                } else if (cmd == "ls") {
                    std::vector<std::string> contents = sys_ls();
                    for (const std::string& item : contents) {
                        CDisplay::TextOut(item.c_str(), 0, y++, BLACK, WHITE);
                    }
                } else if (cmd == "open") {
                    std::string filename;
                    iss >> filename;
                    int fd = sys_open(filename.c_str());
                    if (fd >= 0) {
                        CDisplay::TextOut("File opened with descriptor: ", 0, y, BLACK, WHITE);
                        CDisplay::TextOutInt(fd, 27, y++, BLACK, WHITE);  
                    } else {
                        CDisplay::TextOut("Failed to open file.", 0, y++, BLACK, WHITE);
                    }
                } else if (cmd == "read") {
                    int fd, size;
                    iss >> fd >> size;
                    char buffer[1024];
                    int bytesRead = sys_read(fd, buffer, size);
                    if (bytesRead > 0) {
                        CDisplay::TextOut("Read ", 0, y, BLACK, WHITE);
                        CDisplay::TextOutInt(bytesRead, 5, y, BLACK, WHITE);  
                        CDisplay::TextOut(" bytes: ", 8, y, BLACK, WHITE);
                        CDisplay::TextOut(std::string(buffer, bytesRead).c_str(), 15, y++, BLACK, WHITE);
                    } else {
                        CDisplay::TextOut("Failed to read file.", 0, y++, BLACK, WHITE);
                    }
                } else if (cmd == "write") {
                    int fd;
                    std::string data;
                    iss >> fd;
                    std::getline(iss, data); 
                    int bytesWritten = sys_write(fd, data.c_str(), data.size());
                    if (bytesWritten > 0) {
                        CDisplay::TextOut("Wrote ", 0, y, BLACK, WHITE);
                        CDisplay::TextOutInt(bytesWritten, 6, y++, BLACK, WHITE);
                        CDisplay::TextOut(" bytes.", 12, y++, BLACK, WHITE);
                    } else {
                        CDisplay::TextOut("Failed to write to file.", 0, y++, BLACK, WHITE);
                    }
                } else if (cmd == "close") {
                    int fd;
                    iss >> fd;
                    if (sys_close(fd) == 0) {
                        CDisplay::TextOut("File descriptor closed.", 0, y++, BLACK, WHITE);
                    } else {
                        CDisplay::TextOut("Failed to close file descriptor.", 0, y++, BLACK, WHITE);
                    }
                } else if (cmd == "unlink") {
                    std::string filename;
                    iss >> filename;
                    if (sys_unlink(filename.c_str()) == 0) {
                        CDisplay::TextOut("File deleted.", 0, y++, BLACK, WHITE);
                    } else {
                        CDisplay::TextOut("Failed to delete file.", 0, y++, BLACK, WHITE);
                    }
                } else {
                    CDisplay::TextOut("Unknown command: ", 0, y, BLACK, WHITE);
                    CDisplay::TextOut(cmd.c_str(), 16, y++, BLACK, WHITE);
                }

                input.clear(); //
                if (y > 24) { 
                    CDisplay::ClearScreen();
                    y = 0;
                }

                CDisplay::TextOut("shell> ", 0, y, BLACK, WHITE, true); 
            } else if (key == '\b' && !input.empty()) {
                input.pop_back(); 
            } else {
                input.push_back(key);
                CDisplay::TextOut(std::string(1, key).c_str(), 7 + input.size(), y, BLACK, WHITE, true); 
            }
        }
    }
}
