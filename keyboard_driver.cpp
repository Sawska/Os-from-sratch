#include "keyboard_driver.h"
#include "PortIO.h"
#include <queue>
#include <vector>
#include <iostream>


#define KEYBOARD_PORT 0x60
#define MOUSE_PORT 0x60
#define KEYBOARD_INTERRUPT 0x21 
