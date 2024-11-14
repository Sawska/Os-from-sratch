#include "keyboard_driver.h"
#include "PortIO.h"
#include <queue>
#include <vector>
#include <iostream>
#include "CDisplay.h"
#include <stdint.h>
#include <sstream>


#define KEYBOARD_PORT 0x60
#define MOUSE_PORT 0x60
#define KEYBOARD_INTERRUPT 0x21 
#define MOUSE_INTERRUPT 0x2C    
std::unordered_map<uint8_t, void (*)()> interruptVectorTable;

int y = 0;

extern "C" void keyboardInterruptHandler() {
    uint8_t scancode = inb(KEYBOARD_PORT);
    
    char key = processScancode(scancode);
    shellInputBuffer.push(key);           
}


void initializeMouse() {
    outb(0x64, 0xA8);
    outb(0x64, 0x20);
    uint8_t status = inb(0x60) | 2;
    outb(0x64, 0x60);  
    outb(0x60, status); 
}


extern "C" void mouseInterruptHandler() {
    static uint8_t mouseCycle = 0;
    static int8_t mouseData[3];

    mouseData[mouseCycle++] = inb(MOUSE_PORT);  
    if (mouseCycle == 3) {
        processMouseData(mouseData);  
        mouseCycle = 0;  
    }
}


class DeviceDriver {
public:
    virtual void initialize() = 0;
    virtual void handleInterrupt() = 0;
    virtual ~DeviceDriver() = default;
};



class KeyboardDriver : public DeviceDriver {
public:
    void initialize() override {

        registerInterruptHandler(KEYBOARD_INTERRUPT, &keyboardInterruptHandler);
    }
    void handleInterrupt() override {

    }
};


class MouseDriver : public DeviceDriver {
public:
    void initialize() override {
        initializeMouse();  
        
        registerInterruptHandler(MOUSE_INTERRUPT, &mouseInterruptHandler);
    }
    void handleInterrupt() override {

    }
};


void initializeDrivers() {
    KeyboardDriver keyboard;
    MouseDriver mouse;

    keyboard.initialize();
    mouse.initialize();
    

}


void registerInterruptHandler(uint8_t interruptNumber, void (*handler)()) {
    interruptVectorTable[interruptNumber] = handler;
}


char processScancode(uint8_t scancode) {
    switch (scancode) {
        case 0x1E: return 'A'; case 0x30: return 'B'; case 0x2E: return 'C'; case 0x20: return 'D';
        case 0x12: return 'E'; case 0x21: return 'F'; case 0x22: return 'G'; case 0x23: return 'H';
        case 0x17: return 'I'; case 0x24: return 'J'; case 0x25: return 'K'; case 0x26: return 'L';
        case 0x32: return 'M'; case 0x31: return 'N'; case 0x18: return 'O'; case 0x19: return 'P';
        case 0x10: return 'Q'; case 0x13: return 'R'; case 0x1F: return 'S'; case 0x14: return 'T';
        case 0x16: return 'U'; case 0x2F: return 'V'; case 0x11: return 'W'; case 0x2D: return 'X';
        case 0x15: return 'Y'; case 0x2C: return 'Z';

        case 0x02: return '1'; case 0x03: return '2'; case 0x04: return '3'; case 0x05: return '4';
        case 0x06: return '5'; case 0x07: return '6'; case 0x08: return '7'; case 0x09: return '8';
        case 0x0A: return '9'; case 0x0B: return '0';

        case 0x39: return ' ';     
        case 0x1C: return '\n';    
        case 0x0E: return '\b';    

        case 0x1A: return '['; case 0x1B: return ']';
        case 0x27: return ';'; case 0x28: return '\'';
        case 0x33: return ','; case 0x34: return '.';
        case 0x35: return '/';

        case 0x3B: return 'F1'; case 0x3C: return 'F2'; case 0x3D: return 'F3';
        case 0x3E: return 'F4'; case 0x3F: return 'F5'; case 0x40: return 'F6';
        case 0x41: return 'F7'; case 0x42: return 'F8'; case 0x43: return 'F9';
        case 0x44: return 'F10'; case 0x57: return 'F11'; case 0x58: return 'F12';

        default: return '?';  
    }
}

void registerKeyboardInterrupt() {
    registerInterruptHandler(33, []() {
        uint8_t scancode = keyboardPort(); 
        char character = processScancode(scancode);
        if (character != '?') {
            CDisplay::TextOutChar(character,0,y++,BLACK,WHITE);
        }
    });
}

void registerMouseInterrupt() {
    registerInterruptHandler(44, []() {
        int8_t data[3] = {0}; 
        readMousePort(data);  
        processMouseData(data);
    });
}


int mouseX = 40;
int mouseY = 12;

void processMouseData(int8_t* data) {
    bool leftButton = data[0] & 0x01;
    bool rightButton = data[0] & 0x02;
    bool middleButton = data[0] & 0x04;

    int8_t xMovement = data[1];
    int8_t yMovement = data[2];

    mouseX += xMovement;
    mouseY -= yMovement;  

    if (mouseX < 0) mouseX = 0;
    if (mouseY < 0) mouseY = 0;
    if (mouseX > 80) mouseX = 80;
    if (mouseY > 24) mouseY = 24;


    if (leftButton) CDisplay::TextOut("Left ",0,y++,BLACK,WHITE);
    if (rightButton) CDisplay::TextOut("Right",0,y++,BLACK,WHITE);
    if (middleButton) CDisplay::TextOut("Middle",0,y++,BLACK,WHITE);
    CDisplay::TextOutChar('\n',0,y++,BLACK,WHITE);

    std::ostringstream oss;
    oss << "Mouse Movement: X = " << xMovement << ", Y = " << yMovement;
    CDisplay::TextOut(oss.str().c_str(), 0, y++, BLACK, WHITE);


    if (leftButton) {
        CDisplay::TextOut("Left button clicked!",0,y++,BLACK,WHITE);
    }
    if (rightButton) {
        CDisplay::TextOut("Right button clicked!",0,y++,BLACK,WHITE);
    }
}


uint8_t keyboardPort() {
    uint8_t scancode;
    __asm__ volatile ("inb %1, %0" : "=a"(scancode) : "Nd"(0x60));
    return scancode;
}


void readMousePort(int8_t* data) {
    for (int i = 0; i < 3; i++) {
        __asm__ volatile ("inb %1, %0" : "=a"(data[i]) : "Nd"(0x60));
    }
}





