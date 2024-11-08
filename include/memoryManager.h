#include <typedefs.h>

#ifndef __TYPEDEF__
typedef unsigned char u8;
typedef unsigned short int u16;
typedef short int s16;
typedef unsigned int u32;
typedef int s32;
typedef unsigned long int u64;
typedef long int s64;
typedef void* ptr;
#define __TYPEDEF__
#endif

#ifndef _MEMORYMANAGER_H_
#define _MEMORYMANAGER_H_

extern u8 kernelheap_start;

//!! kernelheap_size will not be the size of the heap. You cant actually pass in variables from the linker script. The size will be the addres of kernelheap_size
extern u8 kernelheap_size;

#define KERNEL_HEAP_SIZE (&kernelheap_size)


//the last heapPart in the list will have nextpart as null and end as the first byte after the heap
typedef struct HEAPPART {
	ptr start;
	u64 size;
	ptr nextpart;
	u8 used;
	u8 extra;	//extra members are not currently used
	u8 extra1;
	u8 extra2;
	u32 extra3;
} __attribute__((aligned(8), packed))heapPart;

heapPart* kheap_first;


s32 mmKernelHeapInit() {
	kheap_first = (heapPart*) &kernelheap_start;
	
	//Maybe check if the kernel size is too small here?
	
	kheap_first->start = ( (&kernelheap_start) + sizeof( heapPart ) );
	kheap_first->size = (&kernelheap_size);
	kheap_first->nextpart = (ptr)0;
	kheap_first->used = 0;
	
	return 1;
}

ptr kmalloc(u64 size) {
	heapPart* ptr  = kheap_first;
	while (ptr != (ptr)0) {
		
		
		if ( (ptr->size >= size) ) {
			
			
			if (ptr->size <= (sizeof(heapPart) + size ) ) {
				
				heapPart* ptr1 = kheap_first;
				while (ptr1 != (ptr)0) {
				
					if (ptr1 == ptr) {
						ptr1 = ptr1->nextpart;
						continue;
					}
				
					if (ptr1->size >= sizeof(heapPart) ) {
						
					}
					
				}
				
			}
			else {
				
				u64 i = size + sizeof(heapPart);
				ptr->size = ptr->size - sizeof(heapPart);
				
				*((heapPart*)(ptr->nextpart)) = {
					.start = (ptr->start + i),
					.size = (ptr->size - i),
					.nextpart = ptr->nextpart,
					.used = 0
				};
				
				ptr->nextpart = ptr->start + size;
				
			}
			
		}
		
	}
	return 0;
	
}

#endif