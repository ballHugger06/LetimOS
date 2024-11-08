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
            terminalPutS(stuff, "error", rgb(255,255,255), rgb(0,0,0));
        }
    }

    while(1) {
		for (u32 i = 0; i < 0x00FFFFFF; i++);
		terminalPutS(stuff, " Lorem ipsum dolor sit amet, consectetur adipiscing elit. ", rgb(0,255,255), rgb(100,100,0));
	}
}