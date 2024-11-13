#ifndef KEYBOARD_DRIVER_H
#define KEYBOARD_DRIVER_H

#include <stdint.h>


#define KEYBOARD_PORT 0x60
#define MOUSE_PORT 0x60
#define KEYBOARD_INTERRUPT 0x21  
#define MOUSE_INTERRUPT 0x2C     


extern "C" void keyboardInterruptHandler();  
extern "C" void mouseInterruptHandler();     


void initializeMouse();                     
void registerInterruptHandler(uint8_t interruptNumber, void (*handler)());  


char processScancode(uint8_t scancode);
void processMouseData(int8_t* data);   

std::queue<char> shellInputBuffer;

#endif // KEYBOARD_DRIVER_H
