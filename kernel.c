#include <typedefs.h>
#include <bootboot.h>
#include <terminalDraw.h>
#include <useful.h>
#include <erenstring.h>
#include <asmFunctions.h>

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
	terminalInitForKernel(stuff);
	terminalChangeBackground(stuff, rgb(50, 0, 10));

	char istring[50];
	int2str(checkInterrupts(), istring, 50);
	terminalPutS(stuff, istring, 1, rgb(255,255,255), rgb(0,0,0));
	terminalPutC(stuff, ' ', 1, rgb(255,255,255), rgb(0,0,0));
	int2str(checkInterrupts(), istring, 50);
	terminalPutS(stuff, istring, 1, rgb(255,255,255), rgb(0,0,0));
	haltCpu();

	while (1) {
		terminalPutS(stuff, " niggers ", 0, rgb(255,255,255), rgb(0,0,0));
	}
}
