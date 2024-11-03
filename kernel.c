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
    if (!terminalInitForKernel(&stuff)) {
        while (1) {
            PRINTERROR(300, rgb(100,100,200));
        }
    }

    PRINTERROR(200, rgb(150,225,200));

    while (1) {
        terminalPutS(&stuff, "Salam Dunya");
    }
}