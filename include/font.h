#include <typedefs.h>
#include <tar.h>
#include <bootboot.h>

#ifndef _FONT_
#define _FONT_
typedef struct {
    u32 magic;
    u32 version;
    u32 headersize;
    u32 flags;
    u32 numglyph;
    u32 bytesperglyph;
    u32 height;
    u32 width;
    u8 glyphs;
} __attribute__((packed)) psf2File;
#endif