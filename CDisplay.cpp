#include "CDisplay.h"
#include "CString.h"
#include <stdint.h>

void CDisplay::ClearScreen() {
    unsigned short* videoMemory = (unsigned short*)0xB8000;
    unsigned short blank = (0x0F << 8) | ' ';

    for (int i = 0; i < 80 * 25; i++) {  
        videoMemory[i] = blank;
    }
}

void CDisplay::TextOut(const char* inStrSource, byte inX, byte inY, byte inBackgroundColor, byte inTextColor, bool inUpdateCursor) {
    byte textAttribute = ((inTextColor) | (inBackgroundColor << 4));
    byte lengthOfString = CString::Strlen(inStrSource);
    
    unsigned short* videoMemory = (unsigned short*)0xB8000;
    int offset = inY * 80 + inX;

    for (int i = 0; i < lengthOfString; ++i) {
        videoMemory[offset + i] = (textAttribute << 8) | inStrSource[i];
    }

    if (inUpdateCursor) {
        int cursorPos = offset + lengthOfString;
        outb(0x3D4, 0x0F);
        outb(0x3D5, (unsigned char)(cursorPos & 0xFF));
        outb(0x3D4, 0x0E);
        outb(0x3D5, (unsigned char)((cursorPos >> 8) & 0xFF));
    }
}

void CDisplay::ShowCursor(bool inMode) {
    byte flag = inMode ? 0 : 0x20;

    asm volatile (
        "movb %0, %%ch;"    
        "movb $0x20, %%cl;"  
        "movb $0x01, %%ah;"  
        "int $0x10;"         
        :
        : "r"(flag)          
        : "%eax", "%ebx", "%ecx", "%edx"
    );
}


void CDisplay::TextOutChar(char c, int x, int y, int fgColor, int bgColor) {
    unsigned short* videoMemory = (unsigned short*)0xB8000;
    int offset = y * 80 + x;

    unsigned short attribute = (bgColor << 4) | (fgColor & 0x0F);

    videoMemory[offset] = (attribute << 8) | c;
}


static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

void CDisplay::TextOutInt(int number, int x, int y, int bgColor, int fgColor) {
    std::string numStr = std::to_string(number);
        TextOut(numStr.c_str(), x, y, bgColor, fgColor, false);
    }