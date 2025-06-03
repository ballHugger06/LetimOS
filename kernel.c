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
	mmKernelHeapInit();
	terminalStuff stuffe;
	terminalStuff* stuff = &stuffe;
	if (!terminalInitForKernel(stuff)) {
		while (1) {
			terminalPutS(stuff, "error", 1, rgb(255,255,255), rgb(0,0,0));
		}
	}
	terminalChangeBackground(stuff, rgb(10, 0, 10));

	u8 i = 0;
	u8 j = 85;
	u8 k = 170;
	u8 diff = 0;
	while(1) {
		i += 3;
		j += 4;
		k += 5;
		diff++;
		terminalChangeBackground(stuff, rgb(i, j, k));
		delay();
		delay();
		delay();
		terminalPutS(stuff, " nigga nigga nigga nigga nigga nigga nigga why do police hate niggas? nigga nigga nigga nigga nigga nigga nigga cause our dicks is bigger! ", diff % 2, rgb(0,0,0), rgb(100,100,100));
	}
}
