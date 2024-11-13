#include <typedefs.h>
#include <bootboot.h>
#include <PSF.h>
#include <memoryFunctions.h>
#include <ustar.h>
#include <erenmath.h>
#include <useful.h>

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

typedef struct {
	u8* fb;
	glyphListA glyph_list;
	u32 scanline;
	u32 height;
	u32 width;
	u32 glyph_height;
	u32 glyph_width;
	u32 glyph_size;
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

	u8* file = (u8*)ustarFindFileByPathBBSafe("fonts/font.psf");
	if (file == 0) {
		return 0;
	}

	u64 size = ustarGetFileSize(((ustarHeader*)file));

	file = ustarGetFileStart((ustarHeader*)file);

	stuff->glyph_height = ((psf2Header*)file)->height;
	stuff->glyph_width = ((psf2Header*)file)->width;
	stuff->glyph_size = ((psf2Header*)file)->glyph_size;

	stuff->row = (stuff->height / (stuff->glyph_height));
	stuff->column = (stuff->width / (stuff->glyph_width));
	stuff->cursor_column = 1;
	stuff->cursor_row = 1;

	if (!psf2FillGlyphListA((psf2Header*)file, size, &(stuff->glyph_list))) {
		return 0;
	}
	return 1;
}




//scrolls the terminal by one row
s32 terminalScroll(terminalStuff* stuff) {
	u32 i;
	u64 width_u8 = stuff->width * 4;

	//UNCOMMENT THE LINES BELOW RGB(255,0,0)
	// u8 swap[stuff->glyph_height][width_u8];
	// u8 swap2[stuff->glyph_height][width_u8];

	//THE LINES BELOW ARE FAULTY AND SHOULD NOT BE TRUSTED RGB(255,0,0)
	u8* swap[stuff->glyph_height];
	u8* swap2[stuff->glyph_height];

	for (u32 l = 0; l < stuff->glyph_height; l++) {
		swap[l] = stuff->fb + (l * stuff->scanline);
		swap2[l] = stuff->fb + (stuff->scanline * stuff->glyph_height) + (l * stuff->scanline);
	}
	//THE LINES ABOVE ARE FAULTY AND SHOULD NOT BE TRUSTED RGB(255,0,0)

	u8* pixelrow = stuff->fb + pixelOffset0(0, (stuff->row * stuff->glyph_height) - 1, stuff->scanline);

	//initalizing the swap algorithm
	pixelrow -= stuff->scanline * stuff->glyph_height;
	for (i = 0; i < stuff->glyph_height; i++) {
		memcpy_big(swap[i], pixelrow, width_u8);
		pixelrow -= stuff->scanline;
	}
	pixelrow += stuff->scanline * (stuff->glyph_height * 2);
	
	for (i = 0; i < stuff->glyph_height; i++) {
		memcpy_big(pixelrow - (stuff->glyph_height * stuff->scanline), pixelrow, width_u8);
		pixelrow -= stuff->scanline;
	}
	pixelrow -= stuff->scanline * stuff->glyph_height;

	//now the main algorithm
	//4 SHOULD BE 2!!!!! rgb(255,0,0)
	for (i = 0; i < (stuff->row - 4); i++) {
		u32 j;
		
		if (i % 2) {
			for (j = 0; j < stuff->glyph_height; j++) {
				memcpy_big(swap2[j], pixelrow, width_u8);
				pixelrow-= stuff->scanline;
			}
			pixelrow += stuff->scanline * (stuff->glyph_height);

			for (j = 0; j < stuff->glyph_height; j++) {
				memcpy_big(pixelrow, swap[j], width_u8);
				pixelrow -= stuff->scanline;
			}
		}
		else {
			for (j = 0; j < stuff->glyph_height; j++) {
				memcpy_big(swap[j], pixelrow, width_u8);
				pixelrow-= stuff->scanline;
			}
			pixelrow += stuff->scanline * (stuff->glyph_height);
			
			for (j = 0; j < stuff->glyph_height; j++) {
				memcpy_big(pixelrow, swap2[j], width_u8);
				pixelrow -= stuff->scanline;
			}
		}
	}

	return 1;
}



//puts c at the cursor position and increments the position
s32 terminalPutC(terminalStuff* stuff, char c, u32 color, u32 background_color) {
	u8* pixel = stuff->fb + pixelOffset0((stuff->glyph_width * (stuff->cursor_column - 1)), ((stuff->glyph_height * (stuff->cursor_row - 1))), stuff->scanline);

	//i is glyph index, k is width index in bytes, j is glyph bit and pixel index
	for (u32 k = 0, i = 0; i < stuff->glyph_size; i++) {
		k = (i % (((stuff->glyph_width - 1) / 8) + 1));

		for (u8 j = 0; j < 8; j++) {
			if ((u8)(((u8)(((stuff->glyph_list)[c])[i])) << j) >= ((u8)0b10000000)) {
				*(u32*)(pixel + (32 * k) + (4 * j)) = color;
			}
			else {
				*(u32*)(pixel + (32 * k) + (4 * j)) = background_color;
			}

			if (((k * 8) + (j + 1)) == stuff->glyph_width) {
				pixel += stuff->scanline;
				break;
			}
		}
		
	}
	
	stuff->cursor_column += 1;
	if (stuff->cursor_column > stuff->column) {
		stuff->cursor_column = 1;
		stuff->cursor_row += 1;

		if (stuff->cursor_row > stuff->row) {
			terminalScroll(stuff);
			stuff->cursor_row = stuff->row;
		}
	}
	return 1;
}

s32 terminalPutS(terminalStuff* stuff, char* s, u32 color, u32 background_color) {
	u64 i = 0;
	while (s[i] != 0) {
		terminalPutC(stuff, s[i], color, background_color);
		i++;
	}
	return 1;
}
#endif
