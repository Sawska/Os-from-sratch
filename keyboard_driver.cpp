#include "keyboard_driver.h"
#include "PortIO.h"
#include <queue>
#include <vector>
#include <iostream>


#define KEYBOARD_PORT 0x60
#define MOUSE_PORT 0x60
#define KEYBOARD_INTERRUPT 0x21 
#define MOUSE_INTERRUPT 0x2C    
std::queue<char> shellInputBuffer;
std::unordered_map<uint8_t, void (*)()> interruptVectorTable;



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
        case 0x1E: return 'A';  
        case 0x30: return 'B';  
        case 0x2E: return 'C';  
        case 0x20: return 'D';  

        case 0x1C: return 'E';  

        default: return '?';  
    }
}



void processMouseData(int8_t* data) {
    
    bool leftButton = data[0] & 0x01;
    bool rightButton = data[0] & 0x02;
    bool middleButton = data[0] & 0x04;

    
    int8_t xMovement = data[1];
    int8_t yMovement = data[2];


    std::cout << "Mouse Buttons: ";
    std::cout << (leftButton ? "Left " : "") << (rightButton ? "Right " : "") << (middleButton ? "Middle " : "") << std::endl;


    std::cout << "Mouse Movement: X = " << (int)xMovement << ", Y = " << (int)yMovement << std::endl;

    if (leftButton) {
        std::cout << "Left button clicked!" << std::endl;
    }
    if (rightButton) {
        std::cout << "Right button clicked!" << std::endl;
    }
}
