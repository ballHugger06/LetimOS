#include <typedefs.h>
#include <bootboot.h>

#ifndef _USTAR_H_
#define _USTAR_H_
typedef struct {
	char filename[100];
	char filemode[8];
	char userid[8];
	char groupid[8];
	char filesize[12];
	char mtime[12];
	char checksum[8];
	char type;
	char linkedfilename[100];
	char indicator[6];
	char version[2];
	char ownername[32];
	char ownergroupname[32];
	char devicemajornum[8];
	char deviceminornum[8];
	char filenameprefix[155];
} ustarHeader;

u64 ustarGetFileSize(ustarHeader* header) {
    u64 size = 0;
    u32 j;
    u32 count = 1;

    for (j = 11; j > 0; j--, count *= 8) {
        size += ((header->filesize[j - 1] - '0') * count);
    }

    return size;
}

u8* ustarGetFileStart(ustarHeader* start) {
	return (((u8*)start) + 512);
}

ustarHeader* ustarGetNextHeaderBBSafe(ustarHeader* start) {
	u64 size = ustarGetFileSize(start);
	ustarHeader* ptr = (ustarHeader*)((u8*)start + ((((size + 511) / 512) + 1) * 512));
	if ((u64)ptr >= bootboot.initrd_ptr + bootboot.initrd_size) {
		return 0;
	}
	return ptr;
}

ustarHeader* ustarFindFileByPathBBSafe(char* path) {
	ustarHeader* ptr = (ustarHeader*)bootboot.initrd_ptr;

	while(1) {
		ptr = ustarGetNextHeaderBBSafe(ptr);
		if (ptr == 0) {
			return 0;
		}

		if (strsame(path, ptr->filename)) {
			return ptr;
		}


		for (u32 i = 0; i < bootboot.fb_height; i++) {
		    *(u32*)(&fb + ( ( ( i ) - 1 ) * bootboot.fb_scanline ) + ( 200 * 4 )) = 0x000FFFF0;
		}
		
	}
}

#endif




