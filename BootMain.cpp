#include "CDisplay.h"
#define HELLO_STR  "\"Hello, world…\", from low-level..."
#include "Interrupts.h"
extern "C" void BootMain()
{
    CDisplay::ClearScreen();
    CDisplay::ShowCursor(false);
    CDisplay::TextOut(
        HELLO_STR,
        0,
        0,
        BLACK,
        WHITE,
        false
    );

    InitializeIDT();

    
    asm volatile ("sti");

 
    while (true) {
 
    }
    return;
}