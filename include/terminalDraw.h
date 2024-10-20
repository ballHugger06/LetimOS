#include <typedefs.h>
#include <bootboot.h>
#include <PSF.h>
#include <memoryFunctions.h>


#ifndef _BOOTBOOT_
#define _BOOTBOOT_
extern BOOTBOOT bootboot;
extern u8 fb;
extern char environment[4096];
#endif


#ifndef _TERMINALWRITE_
#define _TERMINALWRITE_

typedef struct {
	u8* fb;
	glyphListA glyph_list;
	u32 scanline;
	u32 height;
	u32 width;
	u8 glyph_height;
	u8 glyph_width;
	u16 row;
	u16 column;
	u16 cursor_row;
	u16 cursor_column;
} terminalStuff;

//initializes the terminal for the kernel to use
//returns 1 for succes, 0 for fail
s32 terminalInitForKernel(terminalStuff* stuff) {
	stuff->fb = &fb;
	
	ptr file = tarFindFileByPathBBSafe("/font/font.psf");
	if (file == 0) {
		return 0;
	}

	file = tarGetFileStart(file);

}

#endif