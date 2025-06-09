#include <typedefs.h>
#include <bootboot.h>
#include <PSF.h>
#include <memoryFunctions.h>
#include <memoryManager.h>
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
	u32 color;
	u32 background_color;
	char c;
	u8 different_background;
} __attribute__((aligned)) terminalChar;

typedef struct {
	terminalChar* stringptr;
	u8* fb;
	glyphListA glyph_list;
	u32 scanline;
	u32 height;
	u32 width;
	u32 glyph_height;
	u32 glyph_width;
	u32 glyph_size;
	u32 background_color;
	s32 string_index; //stringptr[string_index] is the last filled character and -1 means the screen is not filled
	u16 row;
	u16 column;
	u16 cursor_row;
	u16 cursor_column;
} __attribute__((aligned)) terminalStuff ;


void terminalUpdateFB(terminalStuff*);

//initializes the terminal for the kernel to use
//returns 1 for succes, 0 for fail
s32 terminalInitForKernel(terminalStuff* stuff) {
	stuff->fb = &fb;
	stuff->height = bootboot.fb_height;
	stuff->width = bootboot.fb_width;
	stuff->scanline = bootboot.fb_scanline;
	stuff->stringptr = kmalloc(stuff->row * stuff->column * sizeof(terminalChar));
	memzero((u8*)stuff->stringptr, stuff->row * stuff->column * sizeof(terminalChar));
	stuff->string_index = -1;

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



//changes the terminal background color
void terminalChangeBackground(terminalStuff* stuff, u32 color) {
	stuff->background_color = color;
	terminalUpdateFB(stuff);
}

//the scrapped line will be written to buffer unless the pointer is null
void terminalScrollString(terminalStuff* stuff, char* buffer) {
	u64 i = 0;
	while (1) {
		if ((i == stuff->column - 1) || (stuff->stringptr[i].c == '\n') || (stuff->stringptr[i].c == '\0')) break;
		i++;
	}

	for (u64 j = 0; j < (stuff->string_index + 1 ) - (i + 1); j++) {
		stuff->stringptr[j] = stuff->stringptr[j + i + 1];
		stuff->stringptr[j + i + 1].c = '\0';
		stuff->stringptr[j + i + 1].background_color = stuff->background_color;
	}

	stuff->string_index -= i + 1;
	return;
}

//scrolls the terminal by one row (framebuffer stuff, stopped using it)
s32 terminalScroll(terminalStuff* stuff) {
	u32 i;
	u64 width_u8 = stuff->width * 4;

	u8* swap[stuff->glyph_height];
	u8* swap2[stuff->glyph_height];

	for (u16 counter = 0; counter < stuff->glyph_height; counter++) {
		swap[counter] = kmalloc(stuff->width * 4);
		swap2[counter] = kmalloc(stuff->width * 4);
		if ( (swap[counter] == 0)) {
			PRINTERROR(counter + 1, rgb(0,255,0));
		}
	}


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
	for (i = 0; i < (stuff->row - 2); i++) {
		u32 j;
		
		if (i % 2) {
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
		else {
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
	}

	for (u16 counter = 0; counter < stuff->glyph_height; counter++) {
		kfree(swap[counter]);
		kfree(swap2[counter]);
	}

	return 1;
}



//puts c at the cursor position and increments the position
//if changebackground is nonzero, paints the background of the char as background_color
s32 terminalPutC(terminalStuff* stuff, char c, char changebackground, u32 color, u32 background_color) {
	u8* pixel = stuff->fb + pixelOffset0((stuff->glyph_width * (stuff->cursor_column - 1)), ((stuff->glyph_height * (stuff->cursor_row - 1))), stuff->scanline);

	if ((c != '\n') && (c != '\0')) {
		//i is glyph index, k is width index in bytes, j is glyph bit/pixel index
		for (u32 k = 0, i = 0; i < stuff->glyph_size; i++) {
			k = (i % (((stuff->glyph_width - 1) / 8) + 1));

			for (u8 j = 0; j < 8; j++) {
				if ((u8)(((u8)(((stuff->glyph_list)[c])[i])) << j) >= ((u8)0b10000000)) {
					*(u32*)(pixel + (32 * k) + (4 * j)) = color;
				}
				else {
					*(u32*)(pixel + (32 * k) + (4 * j)) = (background_color * (changebackground != 0)) + (stuff->background_color * (changebackground == 0));
				}

				if (((k * 8) + (j + 1)) == stuff->glyph_width) {
					pixel += stuff->scanline;
					break;
				}
			}

		}
	}

	stuff->string_index++;
	stuff->stringptr[stuff->string_index].c = c;
	stuff->stringptr[stuff->string_index].color = color;
	if (changebackground) {
		stuff->stringptr[stuff->string_index].background_color = background_color;
		stuff->stringptr[stuff->string_index].different_background = 1;
	}
	else {
		stuff->stringptr[stuff->string_index].different_background = 0;
	}
	stuff->cursor_column += 1;
	if (stuff->cursor_column > stuff->column) {
		stuff->cursor_column = 1;
		stuff->cursor_row += 1;

		if (stuff->cursor_row > stuff->row) {
			terminalScrollString(stuff, 0);
			terminalUpdateFB(stuff);
			stuff->cursor_row = stuff->row;
		}
	}
	return 1;
}

s32 terminalPutS(terminalStuff* stuff, char* s, char changebackground, u32 color, u32 background_color) {
	u64 i = 0;
	while (s[i] != 0) {
		terminalPutC(stuff, s[i], changebackground, color, background_color);
		i++;
	}
	return 1;
}

//clears the terminal screen to color
void terminalClearScreen(terminalStuff* stuff, u32 color) {
	for (u64 i = 0; i < stuff->height; i++) {
		for (u64 k = 0; k < stuff->width; k++) {
			*(u32*)(stuff->fb + pixelOffset0(k, i, stuff->scanline)) = color;
		}
	}
}

//updates the fb on account of the terminal string
void terminalUpdateFB(terminalStuff* stuff) {
	terminalClearScreen(stuff, stuff->background_color);
	stuff->cursor_row = 1;
	stuff->cursor_column = 1;
	s32 loopcount = stuff->string_index + 1;
	stuff->string_index = -1;

	for (u64 i = 0; i < loopcount; i++) {
		terminalPutC(stuff, stuff->stringptr[i].c, stuff->stringptr[i].different_background, stuff->stringptr[i].color, stuff->stringptr[i].background_color);
	}
}
	
#endif
