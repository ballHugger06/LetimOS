#include <typedefs.h>

#ifndef _MEMORYMANAGER_H_
#define _MEMORYMANAGER_H_

extern u8 kernelheap_start;

//!! this is not the size of the heap! You cant actually pass in variables from the linker script. The size will be the addres of kernelheap_size
extern u8 kernelheap_size;

#define KERNEL_HEAP_SIZE ((u64)(&kernelheap_size))

/*
	every allocated memory part has a header. this header is the HEAPPART struct.
	the size member includes the size of the heapPart structure
	
	unallocated memory
		unallocated spaces in memory dont have any headers.
		this will give us the size of free memory between these parts. This calculation is only possible because memory parts are placed paralell to their address orders in the linked list

		not giving free spaces headers also deducts the requirement of an "is_used" member which could create allignement and size problems
*/

//the last heapPart in the list will have nextpart as null and end as the first byte after the heap
typedef struct HEAPPART {
	u64 size;
	ptr nextpart;
} __attribute__((aligned(8), packed))heapPart;

heapPart* kheap_first;


s32 mmKernelHeapInit() {
	kheap_first = (heapPart*) &kernelheap_start;
	
	//Maybe check if the heap size is too small here?
	kheap_first->size = sizeof(heapPart);
	kheap_first->nextpart = (ptr)0;
	
	return 1;
}

ptr kmalloc(u64 size) {
	heapPart* tptr = kheap_first;

	while (tptr != 0) {
		if (tptr->nextpart == 0) {
			if ( ( ( ( (u64)(&kernelheap_start) ) + KERNEL_HEAP_SIZE ) - ( ((u64)tptr) + tptr->size ) ) >= size + sizeof(heapPart) ) {
				heapPart* dptr = (heapPart*)( ((u64)tptr) + tptr->size);

				dptr->size = sizeof(heapPart) + size;
				dptr->nextpart = 0;
				tptr->nextpart = dptr;
				return ((u8*)dptr) + sizeof(heapPart);
			}
		}
		else {
			if ( ( ((u64)( tptr->nextpart )) - ( ((u64)tptr) + tptr->size ) ) >= size + sizeof(heapPart) ) {
				heapPart* dptr = (heapPart*)((u64)tptr + tptr->size);
			
				dptr->size = sizeof(heapPart) + size;
				dptr->nextpart = tptr->nextpart;
				tptr->nextpart = dptr;
				return ((u8*)dptr) + sizeof(heapPart);
			}
		}

		tptr = tptr->nextpart;
	}
	
	return 0;
}

void kfree(ptr p) {
	heapPart* dptr = kheap_first;
	heapPart* tptr = kheap_first->nextpart;

	while (tptr != 0) {
		if ( ((u64)tptr) + sizeof(heapPart) == (u64)p) {
			dptr->nextpart = tptr->nextpart;
			return;
		}

		dptr = tptr;
		tptr = tptr->nextpart;
	}
}

#endif
