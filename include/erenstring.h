#include <typedefs.h>

#ifndef _STRING_
#define _STRING_
s32 strsame(char* first, char* second) {
    if (first[0] != second[0]) {
        return 0;
    }
    u32 i = 1;
    while ((first[i - 1] != 0)) {
        if (first[i] != second[i]) {
            return 0;
        }
        i++;
    }
    return 1;
}
#endif