<<<<<<< HEAD
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

u64 ustarGetFileSize(ustarHeader* file) {
	u8 i = 11;
	u8 j = 1;
	u64 size = 0;
	do {
		size += j * (file->filesize[i] - '0');
		i--;
		j *= 8;
	} while (i != 0);
	return size;
}

ustarHeader* ustarFindFileByPathBBSafe() {
	
}


#endif
=======

ustarHeader* ustarGetNextHeaderBBSafe(ustarHeader* start) {
	u64 size = ustarGetFileSize(start);
	ustarHeader* ptr = ((u8*)start + ((((size + 511) / 512) + 1) * 512));
	if (ptr >= bootboot.initrd_ptr + bootboot.initrd_size) {
		return 0;
	}
	return ptr;
}

ustarHeader* ustarFindFileByPathBBSafe(char* path) {
	ustarHeader* ptr = bootboot.initrd_ptr;

	while(1) {
		ptr = ustarGetNextHeaderBBSafe(ptr);
		if (ptr == 0) {
			return 0;
		}

		if (strsame(path, ptr->filename)) {
			return ptr;
		}
	}
}
>>>>>>> e43b2cf45b3fc5c26745bc77e02525d6feea14f9
