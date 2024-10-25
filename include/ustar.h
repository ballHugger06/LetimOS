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