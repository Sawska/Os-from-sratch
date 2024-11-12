#include "Interrupts.h"
#include "CDisplay.h"
#include "PortIO.h"





struct IDTEntry {
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
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
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


void SetIDTEntry(int n, unsigned int handler, unsigned short sel, unsigned char flags) {
    idt[n].base_low = handler & 0xFFFF;
    idt[n].sel = sel;
    idt[n].always0 = 0;
    idt[n].flags = flags;
    idt[n].base_high = (handler >> 16) & 0xFFFF;
}


void InitializeIDT() {
    idt_reg.base = (unsigned int)&idt;
    idt_reg.limit = sizeof(idt) - 1;

    SetIDTEntry(0, (unsigned int)ISR0, 0x08, 0x8E); 
    SetIDTEntry(33, (unsigned int)IRQ1_Handler, 0x08, 0x8E); 

    
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


extern "C" void isr_handler(uint32_t int_number) {
    if (int_number < 32) {
        switch (int_number) {
            case 0:
                CDisplay::TextOut("Exception: Division by zero", 0, 1, BLACK, RED, false);
                while (1); 
            default:
                CDisplay::TextOut("Unhandled Exception: ", 0, 1, BLACK, RED, false);
                CDisplay::TextOutInt(int_number, 20, 1, BLACK, RED);
                while (1);
        }
    } else if (int_number >= 32 && int_number < 48) {
        uint8_t irq_number = int_number - 32;

        if (irq_number == 1) {
            IRQ1_Handler();
        }
        outb(0x20, 0x20);
        if (irq_number >= 8) {
            outb(0xA0, 0x20);
        }
    }
}
