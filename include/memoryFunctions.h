#include <typedefs.h>

#ifndef _MEMFUNCTIONS_
#define _MEMFUNCTIONS_

void memcpy_big(u8* destination, u8* source, u32 size) {
    u32 j;
                          //8 is the size of a u64
    for (j = 0; j < (size / 8); j++) {
        ((u64*)destination)[j] = ((u64*)source)[j];
    }

    for (char i = 0; i < (size - (8 * (size / 8))); i++) {
        destination[(j * 8) - 8 + i] = source[(j * 8) - 8 + i];
    }
}

#endif