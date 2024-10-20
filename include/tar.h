#include <typedefs.h>
#include <bootboot.h>
#include <erenstring.h>

#ifndef _BOOTBOOT_
#define _BOOTBOOT_
extern BOOTBOOT bootboot;
extern u8 fb;
extern char environment[4096];
#endif

#ifndef _TAR_
#define _TAR_

typedef struct {
    char filename[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag[1];
} tarHeader;

//input is tarHeader.size
u64 tarGetFileSize(char *in) {
    u64 size = 0;
    u32 j;
    u32 count = 1;

    for (j = 11; j > 0; j--, count *= 8) {
        size += ((in[j - 1] - '0') * count);
    }

    return size;
}

//address is the starting address of the current file header
//IMPORTANT:
//check if the ramdisk initrd_size boundry has been passed after using this function
//tarGetNextHeaderBBSafe does this for you with bootboot structure's initrd_size member
tarHeader* tarGetNextHeader(tarHeader* address) {
    u32 size = tarGetFileSize(address->size);
    address += (((size) / 512) + 1) * 512;
    if (size % 512) {
        address += 512;
    }
    return address;
}

//see tarGetNextHeader
tarHeader* tarGetNextHeaderBBSafe(tarHeader* address) {
    tarHeader* swap = tarGetNextHeader(address);
    if (swap >= bootboot.initrd_ptr + bootboot.initrd_size) {
        return 0;
    }
    return swap;
}

//returns the pointer to the file with the filename "path"
//if it isnt found, returns 0
tarHeader* tarFindFileByPathBBSafe(char* path) {
    tarHeader* start = (tarHeader*)bootboot.initrd_ptr;

    while (1) {
        if (strsame(start->filename, path)) {
            return start;
        }
        start = tarGetNextHeaderBBSafe(start);
        if (start == 0) {
            return 0;
        }
    }
}

//dont input wrong addresses
char* tarGetFileStart(tarHeader* header) {
    return header + 512;
}

#endif