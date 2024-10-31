#include "CDisplay.h"
#include "CString.h"

void CDisplay::ClearScreen() {
    __asm {
        mov ah, 0x0C 
        int 0x10
    }
}

void CDisplay::TextOut(const char* inStrSource, byte inX, byte inY, byte inBackgroundColor, byte inTextColor, bool inUpdateCursor)
{
byte textAttribute = ((inTextColor) | (inBackgroundColor << 4));
    byte lengthOfString = CString::Strlen(inStrSource);

    __asm {
        push bp
        mov al, inUpdateCursor
        xor bh, bh
        mov bl, textAttribute
        xor cx, cx
        mov cl, lengthOfString
        mov dh, inY
        mov dl, inX
        mov es, word ptr[inStrSource + 2]
        mov bp, word ptr[inStrSource]
        mov ah, 0x0E  
        int 0x10
        pop bp
    }
}

void CDisplay::ShowCursor(bool inMode) {
    byte flag = inMode ? 0 : 0x20;
    __asm {
        mov ch, flag
        mov cl, 0x20
        mov ah, 0x01  
        int 0x10
    }
}
