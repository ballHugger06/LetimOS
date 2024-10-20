#include <typedefs.h>

#ifndef _BOOTBOOT_H_
#define _BOOTBOOT_H_

#ifdef  __cplusplus
extern "C" {
#endif
#ifndef _MSC_VER
#define _pack __attribute__((packed))
#else
#define _pack
#pragma pack(push)
#pragma pack(1)
#endif

#define BOOTBOOT_MAGIC "BOOT"

/* default virtual addresses for level 0 and 1 static loaders */
#define BOOTBOOT_MMIO   0xfffffffff8000000  /* memory mapped IO virtual address */
#define BOOTBOOT_FB     0xfffffffffc000000  /* frame buffer virtual address */
#define BOOTBOOT_INFO   0xffffffffffe00000  /* bootboot struct virtual address */
#define BOOTBOOT_ENV    0xffffffffffe01000  /* environment string virtual address */
#define BOOTBOOT_CORE   0xffffffffffe02000  /* core loadable segment start */

/* minimum protocol level:
 *  hardcoded kernel name, static kernel memory addresses */
#define PROTOCOL_MINIMAL 0
/* static protocol level:
 *  kernel name parsed from environment, static kernel memory addresses */
#define PROTOCOL_STATIC  1
/* dynamic protocol level:
 *  kernel name parsed, kernel memory addresses from ELF or PE symbols */
#define PROTOCOL_DYNAMIC 2
/* big-endian flag */
#define PROTOCOL_BIGENDIAN 0x80

/* loader types, just informational */
#define LOADER_BIOS     (0<<2)
#define LOADER_UEFI     (1<<2)
#define LOADER_RPI      (2<<2)
#define LOADER_COREBOOT (3<<2)

/* framebuffer pixel format, only 32 bits supported */
#define FB_ARGB   0
#define FB_RGBA   1
#define FB_ABGR   2
#define FB_BGRA   3

/* mmap entry, type is stored in least significant tetrad (half u8) of size
 * this means size described in 16 u8 units (not a problem, most modern
 * firmware report memory in pages, 4096 u8 units anyway). */
typedef struct {
  u64   ptr;
  u64   size;
} _pack MMapEnt;
#define MMapEnt_Ptr(a)  ((a)->ptr)
#define MMapEnt_Size(a) ((a)->size & 0xFFFFFFFFFFFFFFF0)
#define MMapEnt_Type(a) ((a)->size & 0xF)
#define MMapEnt_IsFree(a) (((a)->size&0xF)==1)

#define MMAP_USED     0   /* don't use. Reserved or unknown regions */
#define MMAP_FREE     1   /* usable memory */
#define MMAP_ACPI     2   /* acpi memory, volatile and non-volatile as well */
#define MMAP_MMIO     3   /* memory mapped IO region */

#define INITRD_MAXSIZE 16 /* Mb */

typedef struct {
  /* first 64 u8s is platform independent */
  u8    magic[4];    /* 'BOOT' magic */
  u32   size;        /* length of bootboot structure, minimum 128 */
  u8    protocol;    /* 1, static addresses, see PROTOCOL_* and LOADER_* above */
  u8    fb_type;     /* framebuffer type, see FB_* above */
  u16   numcores;    /* number of processor cores */
  u16   bspid;       /* Bootsrap processor ID (Local APIC Id on x86_64) */
  s16    timezone;    /* in minutes -1440..1440 */
  u8    datetime[8]; /* in BCD yyyymmddhhiiss UTC (independent to timezone) */
  u64   initrd_ptr;  /* ramdisk image position and size */
  u64   initrd_size;
  u64   fb_ptr;      /* framebuffer pointer and dimensions */
  u32   fb_size;
  u32   fb_width;
  u32  fb_height;
  u32   fb_scanline;

  /* the rest (64 u8s) is platform specific */
  union {
    struct {
      u64 acpi_ptr;
      u64 smbi_ptr;
      u64 efi_ptr;
      u64 mp_ptr;
      u64 unused0;
      u64 unused1;
      u64 unused2;
      u64 unused3;
    } x86_64;
    struct {
      u64 acpi_ptr;
      u64 mmio_ptr;
      u64 efi_ptr;
      u64 unused0;
      u64 unused1;
      u64 unused2;
      u64 unused3;
      u64 unused4;
    } aarch64;
  } arch;

  /* from 128th u8, MMapEnt[], more records may follow */
  MMapEnt    mmap;
  /* use like this:
   * MMapEnt *mmap_ent = &bootboot.mmap; mmap_ent++;
   * until you reach bootboot->size, while(mmap_ent < bootboot + bootboot->size) */
} _pack BOOTBOOT;

#ifdef _MSC_VER
#pragma pack(pop)
#endif

#ifdef  __cplusplus
}
#endif

#ifndef _BOOTBOOT_
#define _BOOTBOOT_
extern BOOTBOOT bootboot;
extern u8 fb;
extern char environment[4096];
#endif

#endif
