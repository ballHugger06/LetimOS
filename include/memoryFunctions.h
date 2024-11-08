#include <typedefs.h>

#ifndef _MEMFUNCTIONS_
#define _MEMFUNCTIONS_

void memcpy_big(u8* destination, u8* source, u64 size) {
    u32 j;
    //8 is the size of a u64
    for (j = 0; j < (size / 8); j++) {
        ((u64*)destination)[j] = ((u64*)source)[j];
    }

    for (u8 i = 0; i < (size - (8 * (size / 8))); i++) {
        destination[(((j + 1) * 8) - 1) + i] = source[(((j + 1) * 8) - 1) + i];
    }
}

#endif