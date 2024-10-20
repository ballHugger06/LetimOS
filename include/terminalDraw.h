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

#define CHARACTER_SPACE 1

typedef struct {
	u8* fb;
	glyphListA glyph_list;
	u32 scanline;
	u32 height;
	u32 width;
	u32 glyph_height;
	u32 glyph_width;
	u16 row;
	u16 column;
	u16 cursor_row;
	u16 cursor_column;
} terminalStuff;

//initializes the terminal for the kernel to use
//returns 1 for succes, 0 for fail
s32 terminalInitForKernel(terminalStuff* stuff) {
	stuff->fb = &fb;
	stuff->height = bootboot.fb_height;
	stuff->width = bootboot.fb_width;
	stuff->scanline = bootboot.fb_scanline;

	ptr file = tarFindFileByPathBBSafe("/font/font.psf");
	if (file == 0) {
		return 0;
	}

	u64 size = tarGetFileSize(((tarHeader*)file)->size);

	file = tarGetFileStart(file);

	stuff->glyph_height = ((psf2Header*)file)->height;
	stuff->glyph_width = ((psf2Header*)file)->width;

	stuff->row = (stuff->height / (stuff->glyph_height + 1));
	stuff->column = (stuff->width / (stuff->glyph_width + 1));
	stuff->cursor_column = 0;
	stuff->cursor_row = 0;
	
	if (!psf2FillGlyphListA(file, size, &(stuff->glyph_list))) {
		return 0;
	}

	return 1;
}

//puts c at the cursor position
s32 terminalPutC(terminalStuff* stuff, char c) {
	u32 i;
	u32 j = 0;
	u8 k;
	u8* pixel = stuff->fb + ( ( ( ( stuff->glyph_height + 1 ) * ( stuff->cursor_row - 1 ) ) - 1 ) * stuff->scanline ) + ( ( stuff->cursor_row - 1 ) * ( stuff->glyph_width + 1) * 4 );

	for (i = 0; i < stuff->glyph_height; i++) {
		for (k = 0; ; k++) {
			if ((j * 8) + k == stuff->glyph_width) {
				break;
			}
			
			if ((stuff->glyph_list[j] << k) >= 0b10000000) {
				*(u32*)(pixel + (j * 8) + k) = 0x00FFFFFF;
			}
			
			if (k == 7) {
				k = 0;
				j += 1;
				continue;
			}
		}

		pixel += stuff->scanline
	}
	return 1;
}

terminalPutS(terminalStuff* stuff, char* s) {
	u64 i = 0;
	while (s[i] != 0) {
		terminalPutC(stuff, s[i]);
	}
	return 1;
}
#endif
