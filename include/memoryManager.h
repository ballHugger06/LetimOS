#include <typedefs.h>

#ifndef _MEMORYMANAGER_H_
#define _MEMORYMANAGER_H_

extern u8 kernelheap_start;

//!! this is not the size of the heap! You cant actually pass in variables from the linker script. The size will be the addres of kernelheap_size
extern u8 kernelheap_size;

#define KERNEL_HEAP_SIZE (&kernelheap_size)

/*
	every allocated memory part has a header. this header is the HEAPPART struct.
	the heappart.start member points to the start of this part, which is the start of the heapPart struct
	the size member includes the size of the heapPart structure
	
	unallocated memory
		unallocated spaces in memory dont have any headers. Their sizes are calculated by subtracting the start member of the next part from (currentheappart.start + currentheappart->size)
		this will give us the size of free memory between these parts. This calculation is only possible because memory parts are placed paralell to their address orders in the linked list

		not giving free spaces headers also deducts the requirement of an "is_used" member which could create allignement problems
*/

//the last heapPart in the list will have nextpart as null and end as the first byte after the heap
typedef struct HEAPPART {
	ptr start;
	u64 size; //size decludes the size of this struct, so the actual size used is heapPart.size + sizeof(heapPart)
	heapPart* nextpart;
} __attribute__((aligned(8), packed))heapPart;

heapPart* kheap_first;


s32 mmKernelHeapInit() {
	kheap_first = (heapPart*) &kernelheap_start;
	
	//Maybe check if the heap size is too small here?
	
	kheap_first->start = (&kernelheap_start);
	kheap_first->size = sizeof(heapPart);
	kheap_first->nextpart = (ptr)0;
	
	return 1;
}

ptr kmalloc(u64 size) {
	heapPart* ptr1  = kheap_first;

	while (ptr1 != 0) {
		
		if (!ptr1->nextpart) {
			if ((ptr1->start + ptr1->size) - ())
		}
		else {
			if ( ( (ptr1->start + ptr1->size) - ( (ptr1->nextpart)->start ) ) >= size + sizeof(heapPart) ) {
				
				
				
			}
		}
		
	}
	return 0;
	
}

#endif
