#ifndef CDISPLAY_H
#define CDISPLAY_H

#define BLACK 0x0
#define BLUE 0x1
#define GREEN 0x2
#define CYAN 0x3
#define RED 0x4
#define MAGENTA 0x5
#define BROWN 0x6
#define GREY 0x7
#define DARK_GREY 0x8
#define LIGHT_BLUE 0x9
#define LIGHT_GREEN 0xA
#define LIGHT_CYAN 0xB
#define LIGHT_RED 0xC
#define LIGHT_MAGENTA 0xD
#define LIGHT_BROWN 0xE
#define WHITE 0xF

#include "Types.h"
#include "CString.h"
#include <cstdint>  // Include for uint8_t and uint32_t
#include <string>

class CDisplay {
public:
    static void ClearScreen();
    static void TextOut(
        const char* inStrSource,
        byte inX = 0,
        byte inY = 0,
        byte inBackgroundColor = BLACK,
        byte inTextColor = WHITE,      
        bool inUpdateCursor = false
    );
    static void ShowCursor(bool inMode);
    static void TextOutChar(char c, int x, int y, int fgColor, int bgColor);
    static void TextOutInt(int number, int x, int y, int bgColor, int fgColor);
};

#endif // CDISPLAY_H
