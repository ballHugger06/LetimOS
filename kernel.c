#include <typedefs.h>
#include <bootboot.h>
#include <terminalDraw.h>

#ifndef _BOOTBOOT_
#define _BOOTBOOT_
extern BOOTBOOT bootboot;
extern u8 fb;
extern char environment[4096];
#endif

void _start(void) {
    terminalStuff stuff;
    if (terminalInitForKernel(&stuff) == 0) {
        while (1) {
            for (u32 i = 0; i < bootboot.fb_height; i++) {
                *(u32*)(&fb + ( ( ( i ) - 1 ) * bootboot.fb_scanline ) + ( 200 * 4 )) = 0x00FF0000;
            }
        }
    }

    for (u32 i = 0; i < bootboot.fb_width; i++) {
        *(u32*)(&fb + ( ( ( 100 ) - 1 ) * bootboot.fb_scanline ) + ( i * 4 )) = 0x00FF0000;
    }

    while (1) {
        terminalPutS(&stuff,"Salam Dunya");
    }
}