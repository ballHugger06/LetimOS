#include <typedefs.h>
#include <bootboot.h>
#include <terminalWrite.h>

#ifndef _BOOTBOOT_
#define _BOOTBOOT_
extern BOOTBOOT bootboot;
extern u8 fb;
extern char environment[4096];
#endif

void _start(void) {
    TerminalWriteStructure stuff;
    Terminal_init(&stuff);
    while (1) {
        for (unsigned o = 0; ; o++) {
            for (u32 j = 0; j < 1024; j++) {
                *(u32*)(&fb + ((((384 - 1) * bootboot.fb_scanline) + (4 * j)))) = 0x00FFFFFF;
            }
            for (u32 i = 0; i < 768; i++) {
                *(u32*)(&fb + ((((i - 1) * bootboot.fb_scanline) + (4 * 512)))) = 0x00FFFFFF;
            }
        }
    }
}