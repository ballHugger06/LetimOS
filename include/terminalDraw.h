#include <typedefs.h>
#include <bootboot.h>
#include <PSF.h>
#include <memoryFunctions.h>
#include <ustar.h>

#ifndef _BOOTBOOT_
#define _BOOTBOOT_
extern BOOTBOOT bootboot;
extern u8 fb;
extern char environment[4096];
#endif


#ifndef _TERMINALWRITE_H_
#define _TERMINALWRITE_H_

typedef struct {
	u8 alpha;
	u8 red;
	u8 green;
	u8 blue;
} argbstruct;

//x is x coordinate, y is y and z is scanline
//x and y both start from 1
#define pixelOffset(x, y, z) ( ( ( y ) - 1 ) * z ) + ( x * 4 )

#define rgb(r, g, b) (u32)(b + (g * 256) + (b * 256 * 256))

//x is the x coordinate for the certical line to be printed
//c is color code is hex
#define PRINTERROR(x, c) for (u32 i = 1; i <= bootboot.fb_height; i++) {*(u32*)(&fb + pixelOffset(x, i, bootboot.fb_scanline)) = c;}

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

	ptr file = ustarFindFileByPathBBSafe("fonts/font.psf");
	if (file == 0) {
		while (1) {
			for (u32 i = 0; i < bootboot.fb_height; i++) {
			    *(u32*)(&fb + ( ( ( i ) - 1 ) * bootboot.fb_scanline ) + ( 200 * 4 )) = 0x0000FF00;
			}
		}
		return 0;
	}

	u64 size = ustarGetFileSize(((ustarHeader*)file));

	file = ustarGetFileStart(file);

	stuff->glyph_height = ((psf2Header*)file)->height;
	stuff->glyph_width = ((psf2Header*)file)->width;

	stuff->row = (stuff->height / (stuff->glyph_height + 1));
	stuff->column = (stuff->width / (stuff->glyph_width + 1));
	stuff->cursor_column = 1;
	stuff->cursor_row = 1;
	
	//Passed prior parts

	if (!psf2FillGlyphListA(file, size, &(stuff->glyph_list))) {
		while (1) {
			for (u32 i = 1; i <= bootboot.fb_height; i++) {
			    *(u32*)(&fb + pixelOffset(700, i, bootboot.fb_scanline)) = 0x00FFFFFF;
			}
		}
		return 0;
	}

	for (u32 i = 0; i < bootboot.fb_height; i++) {
	    *(u32*)(&fb + ( ( ( i ) - 1 ) * bootboot.fb_scanline ) + ( 200 * 4 )) = 0x00FF00FF;
	}

	return 1;
}

//scrolls the terminal once
//doesnt zero out the bottom of the rows
s32 terminalScroll(terminalStuff* stuff) {
	u8 swap[stuff->width * 4];
	u8 swap1[stuff->width * 4];

	memcpy_big(swap, ( stuff->fb + ( ( ( ( stuff->glyph_height + 1 ) * ( (stuff->row - 1) - 1 ) ) - 1 ) * stuff->scanline ) ), (4 * stuff->width));
	memcpy_big(( stuff->fb + ( ( ( ( stuff->glyph_height + 1 ) * ( (stuff->row - 1) - 1 ) ) - 1 ) * stuff->scanline ) ), ( stuff->fb + ( ( ( ( stuff->glyph_height + 1 ) * ( (stuff->row) - 1 ) ) - 1 ) * stuff->scanline ) ), (4 * stuff->width));

	u32 i = ((stuff->row) * (stuff->glyph_height + 1) - 2);
	if (i % 2 == 0) {
		do {
			if (i % 2 == 0) {
				memcpy_big(swap1, ( stuff->fb + ( ( (i) - 1 ) * stuff->scanline ) ), (4 * stuff->width));
				memcpy_big(( stuff->fb + ( ( (i) - 1 ) * stuff->scanline ) ), swap, (4 * stuff->width));
			}
			else {
				memcpy_big(swap, ( stuff->fb + ( ( (i) - 1 ) * stuff->scanline ) ), (4 * stuff->width));
				memcpy_big(( stuff->fb + ( ( (i) - 1 ) * stuff->scanline ) ), swap1, (4 * stuff->width));
			}

			i--;
		} while (i != 0);
	}
	else {
		do {
			if (i % 2 == 0) {
				memcpy_big(swap, ( stuff->fb + ( ( (i) - 1 ) * stuff->scanline ) ), (4 * stuff->width));
				memcpy_big(( stuff->fb + ( ( (i) - 1 ) * stuff->scanline ) ), swap1, (4 * stuff->width));
			}
			else {
				memcpy_big(swap1, ( stuff->fb + ( ( (i) - 1 ) * stuff->scanline ) ), (4 * stuff->width));
				memcpy_big(( stuff->fb + ( ( (i) - 1 ) * stuff->scanline ) ), swap, (4 * stuff->width));
			}

			i--;
		} while (i != 0);
	}

	return 1;
}

//puts c at the cursor position and increments the position
s32 terminalPutC(terminalStuff* stuff, char c) {
	u32 i;
	u32 j = 0;
	u8 k;
	u8* pixel = stuff->fb + ( ( ( ( stuff->glyph_height + 1 ) * ( stuff->cursor_row - 1 ) ) - 1 ) * stuff->scanline ) + ( ( stuff->cursor_column - 1 ) * ( stuff->glyph_width + 1) * 4 );

	for (i = 0; i < stuff->glyph_height; i++) {
		for (k = 0; ; k++) {
			if ((j * 8) + k == stuff->glyph_width) {
				break;
			}
			
			if ((*(stuff->glyph_list[c] + j) << k) >= 0b10000000) {
				*(u32*)(pixel + (j * 8) + k) = 0x00FFFFFF;
			}
			
			if (k == 7) {
				k = 0;
				j += 1;
				continue;
			}
		}

		pixel += stuff->scanline;
	}
	
	stuff->cursor_column += 1;
	if (stuff->cursor_column > stuff->column) {
		stuff->cursor_column = 1;
		stuff->cursor_row +=1;

		if (stuff->cursor_row > stuff->row) {
			terminalScroll(stuff);
			stuff->cursor_row = stuff->row;
		}
	}
	return 1;
}

s32 terminalPutS(terminalStuff* stuff, char* s) {
	u64 i = 0;
	while (s[i] != 0) {
		terminalPutC(stuff, s[i]);
	}
	return 1;
}
#endif
