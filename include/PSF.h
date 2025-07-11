#include <typedefs.h>
#include <memoryManager.h>
#include <tar.h>
#include <bootboot.h>
#include <useful.h>

#ifndef _PSF_H_
#define _PSF_H_

#define PSF1_MAGIC 0x0436

#define PSF2_MAGIC 0X864AB572

#define PSF1_MODE512 0x01
#define PSF1_MODEHASTAB 0X02
#define PSF1_MODESEQ 0X04

#define PSF2_HAS_UNICODE_TABLE 0x00000001

/*This type is used to find glyph addresses faster. By filling out each ascii code's index as the corresponding glyph's
starting address, we can skip calculating an ascii character's glyph's address everytime we need it*/
typedef u8* glyphListA[256];

typedef struct {
    u32 magic;
    u32 version;
    u32 headersize;
    u32 font_flags;
    u32 glyph_number;
    u32 glyph_size;
    u32 height;
    u32 width;
} __attribute__((packed)) psf2Header;

typedef struct {
    u16 magic;
    u8 mode;
    u8 glyph_size;
} __attribute__((packed)) psf1Header;


//returns 1 for psf1, 2 for psf2 and 0 if the file isnt a psf file
s32 psfDetectVersion(ptr file_start) {
    if (((psf1Header*)(file_start))->magic == PSF1_MAGIC) {
        return 1;
    }
    else if (((psf2Header*)(file_start))->magic == PSF2_MAGIC) {
        return 2;
    }
    else {
        return 0;
    }
}

//returns the pointer to the first byte of the unicode table, if the file doesnt include one returns 0
u8* psf1GetUniTable(psf1Header* file_start) {
	if ((file_start->mode == PSF1_MODEHASTAB) || (file_start->mode == PSF1_MODESEQ)) {
		return (((u8*)file_start) + 4 + (file_start->glyph_size * 256));
	}
	return 0;
}

//returns the pointer to the first byte of the unicode table, if the file doesnt include one returns 0 
u8* psf2GetUniTable(psf2Header* file_start) {
	if (file_start->font_flags == PSF2_HAS_UNICODE_TABLE) {
		return (((u8*)file_start) + (file_start->headersize) + (file_start->glyph_size * file_start->glyph_number));
	}
	return 0;
}

//returns the corresponding glyphs first byte. If file_size boundary is crossed, returns 0 
u8* psf2GetGlyphASafe(psf2Header* file_start, u64 file_size, char c) {
	u8* table = psf2GetUniTable(file_start);
	u8 swap;
	u32 i = 0;
	u32 glyph_counter = 0;
	do {
		if (((u64)table + i) >= ((u64)file_start + (u64)file_size)) {
			return 0;
		}

		swap = table[i];
		switch (swap) {
		case 0xfe:
			for (u32 j = 1; ; j++) {
				if (((u64)table + i + j) >= ((u64)file_start + (u64)file_size)) {
					return 0;
				}

				if (table[i + j] == 0xff) {
					i += j + 1;
					break;
				}
			}
			glyph_counter++;
			continue;

		case 0xff:
			i++;
			glyph_counter++;
			continue;
		}

		i++;

	} while (swap != c);

	return (((u8*)file_start) + file_start->headersize + (file_start->glyph_size * glyph_counter));
}

//fills out a glyphListA. returns 1 for succes, 0 for fail
s32 psf2FillGlyphListA(psf2Header *file_start, u64 file_size, glyphListA *list) {
	char first = 1;
	ptr nullglyph;
	for (u8 i = 0; i < 128; i++) {
		(*list)[i] = psf2GetGlyphASafe(file_start, file_size, i);

		if ((*list)[i] == 0) {
			if (first) {
				nullglyph = kmalloc(file_start->glyph_size);
				for (u32 k = 0; k < file_start->glyph_size; k++) {
					((u8*)nullglyph)[k] = 0x00;
				}
				first = 0;
			}

			(*list)[i] = nullglyph;
		}
	}
	return 1;
}

#endif