#include <typedefs.h>
#include <bootboot.h>
#include <terminalDraw.h>
#include <useful.h>

#ifndef _BOOTBOOT_
#define _BOOTBOOT_
extern BOOTBOOT bootboot;
extern u8 fb;
extern char environment[4096];
#endif

void _start(void) {
    terminalStuff stuffe;
	terminalStuff* stuff = &stuffe;
    if (!terminalInitForKernel(stuff)) {
        while (1) {
            PRINTERROR(300, rgb(255,255,25));
        }
    }

    terminalPutS(stuff, "Salam Dunya", rgb(0,255,255));
}