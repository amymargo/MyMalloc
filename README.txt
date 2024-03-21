Amy Margolina
Daniel Wang

An implementation of our own versions of the standard library functions malloc() and free(). Unlike the standard implementations, our version also detects common usage errors and reports them.

Test Plan:
	Correctness and Error Testing in genmemtest.c:
	
	Correctness Testing:

testAlign():
All pointers should be 8-byte aligned. If headers are not divisible by 8, the heap is considered misaligned.

testFree():
Frees an object given a pointer to its start. If the pointer does not refer to a freeable object or cannot be reallocated or coalesced, then free is deemed unsuccessful.

testMalloc():
Reserves unallocated memory. If there is sufficient space, it returns a pointer to an object that does not overlap with others. This object remains untouched or moved by the program until freed.

testCoa():
Coalescing should merge adjacent freed memory blocks. If allocating the entire heap after allocating and freeing smaller sections fails, coalescing is unsuccessful.

	Detectable Error Testing:
doubleFree():
An error should be flagged when the same address is freed twice.
malloc with insufficient free memory of the requested size.

freeNonMallocAddress():
An error should be flagged when attempting to free an address not allocated by malloc().

freeOnMiddleAddress():
An error should be flagged when attempting to free a non-header address.

	Performance Testing in memgrind.c:

memgrind.c conducts performance testing on mymalloc.c with the following tests through memgrind.call:

Allocate and immediately free a 1-byte object 120 times.

Allocate 120 1-byte objects, store the pointers in an array, then free the chunks.

Alternately allocate a 1-byte object and free a previously allocated object from an array of 120 pointers, until 120 allocations are made. Free any remaining objects.

Allocate and free increasingly larger chunks of memory using malloc(), increasing the size by 100 each time, until reaching the limit of 4088 bytes.

Allocate and free one 4088 byte object, then allocate 2 2044 byte objects and free them, then 4 1022 byte objects, then 8 511 byte objects, 16 255 byte options, 32 127 byte objects, and finally 64 63 byte objects(double the number of objects and half the number of bytes each time).

Running memgrind.c measures the time taken by mymalloc to complete these performance tests.

	Design Notes:

Definitions in mymalloc.c:

Definitions are included to facilitate adjustments to the total heap size or heap alignment.
Free and Coalesce:

Coalesce is integrated into free to simplify heap structure maintenance without complex iterations. Coalesce merges adjacent free memory spaces upon freeing an object.
Reporting Errors:

Errors are reported through printed error messages formatted as "ERROR: ______".
