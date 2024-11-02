#include "Interrupts.h"
#include "CDisplay.h"
#include "PortIO.h"

struct  IDTEntry
{
    unsigned short base_low;
    unsigned short sel;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_high;
} __attribute__((packed));

struct IDTRegister {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

IDTEntry idt[256];
IDTRegister idt_reg;


const char scanCodeToASCII[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0
};

extern "C" void ISR0() {
    CDisplay::TextOut("Interrupt: Division by zero", 0, 1, BLACK, RED, false);
    while (1);
}


void InitializeIDT() {
    idt_reg.base = (unsigned int)&idt;
    idt_reg.limit = sizeof(idt) - 1;

    asm volatile ("lidt %0" : : "m" (idt_reg));
}

extern "C" void IRQ1_Handler() {
    unsigned char scan_code = inb(0x60);
    
  
    if (scan_code < sizeof(scanCodeToASCII)) {
        char ascii_char = scanCodeToASCII[scan_code];
        
  
        CDisplay::TextOut("Key Pressed: ", 0, 2, BLACK, WHITE, false);
        CDisplay::TextOutChar(ascii_char, 13, 2, BLACK, WHITE);
    }

 
    outb(0x20, 0x20);
}