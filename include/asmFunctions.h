#include <typedefs.h>

#ifndef _ASMFUNCTIONS_H_
#define _ASMFUNCTIONS_H_

typedef struct {
	u64 base;
	u16 limit
} __attribute__((packed)) GDTbl; //GDT base and limit to be pointed to by the psuedo descriptor

typedef u64 segmentDescriptor;

typedef struct {
	u64 lower;
	u64 higher;
} __attribute__((packed)) systemSegmentDescriptor;

extern void loadGDTR(GDTbl*);
extern void breakpnt();
extern void enableInterrupts();
extern void disableInterrupts();
extern u64 getRFLAGS();
extern void haltCpu();

s32 checkInterrupts() {
	return (getRFLAGS() >> 9) % 2;
}

s32 gdtGenerateCodeDescriptor(char D, char L, char P, char DPL, char C);

#endif
