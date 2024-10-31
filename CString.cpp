#include "CString.h"

byte CString::Strlen(const char* inStrSource) {
    byte lengthOfString = 0;

    while (*inStrSource++ != '\0') {
        ++lengthOfString;
    }
    
    return lengthOfString;
}
