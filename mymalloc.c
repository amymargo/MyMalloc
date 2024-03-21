#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mymalloc.h"

#define MEMORY_SIZE_BYTES 4096
#define MEMORY_LENGTH MEMORY_SIZE_BYTES / sizeof(double)
#define HEADER_SIZE 8
#define INTEGER_SIZE sizeof(int)
static double memory[MEMORY_LENGTH];

#ifndef DEBUG
#define DEBUG 0
#endif


#define memoryStart ((char *)memory)

//given a address inserts a header in the next 8 bytes
// first number in header is if the block is free or not
    //1 is free
    //0 is in use
//second number is the total number of bytes including the header
void setHeader(char * block, int freeoruse, int size){
    ((int *)block)[0] = freeoruse;
    ((int *)block)[1] = size;

    if (DEBUG){
        printf("    header changed at %p to %d %d \n", block, freeoruse, size);
    }
}

void *mymalloc(size_t requestedSize, char *sourceFile, int sourceLine){
    int initializationFlag = ((int *)memory)[1];
    if(initializationFlag == 0) {
        if(DEBUG)
            printf("First time malloc or free is called initializationFlag is %d\n", initializationFlag);
        setHeader(memoryStart, 1, MEMORY_SIZE_BYTES);
    }

    if (DEBUG){
        printf("\nmalloc called with %ld bytes at %s %d \n", requestedSize, sourceFile, sourceLine);
        util();
    }

    if (requestedSize == 0){
        printf("ERROR: malloc called with 0 size at %s %d \n",sourceFile, sourceLine);
        return NULL;
    }

    int alignedRequestSize = (requestedSize + (HEADER_SIZE - 1)) & ~(HEADER_SIZE - 1);

    int searchPosition = 0;
    while(1){
        if (searchPosition >= MEMORY_SIZE_BYTES){
            break;
        }
        if (memoryStart[searchPosition] && ((int *)memoryStart)[(searchPosition/INTEGER_SIZE) + 1] >= alignedRequestSize + HEADER_SIZE){
            break;
        }
        else{
            searchPosition += ((int *)memoryStart)[(searchPosition/INTEGER_SIZE) + 1];
        }
    }

    if (searchPosition >= MEMORY_SIZE_BYTES){
        printf("ERROR: not enough memory or memory is corrupted called from %s at %d \n", sourceFile, sourceLine);
        return NULL;
    } 

    if (((int *)memoryStart)[(searchPosition/INTEGER_SIZE) + 1] - alignedRequestSize - HEADER_SIZE != 0){
        setHeader(memoryStart + searchPosition + HEADER_SIZE + alignedRequestSize, 1,  ((int *)memoryStart)[(searchPosition/INTEGER_SIZE) + 1] - alignedRequestSize - HEADER_SIZE);
    }
    setHeader(memoryStart + searchPosition, 0, alignedRequestSize + HEADER_SIZE);
    
    return memoryStart + searchPosition + HEADER_SIZE;
}

void coa(void* prev, void *ptr) {

    int* next = ptr + ((int*)ptr)[1];
    if (DEBUG){
        printf("coa called with %p pointer \n ", ptr); 
    }

    if (prev != NULL && ((int*)prev)[0] == 1) {
        ((int*)prev)[1] = ((int*)prev)[1] + ((int*)ptr)[1];
        ptr = prev;
    }
    if((char*)next < &memoryStart[MEMORY_SIZE_BYTES - 1] && ((int*)next)[0] == 1) {
        ((int*)ptr)[1] = ((int*)ptr)[1] + ((int*)next)[1];
    }

    if (DEBUG)
        util();
    
}

void myfree(void *pointerToFree, char *sourceFile, int sourceLine){

    pointerToFree = pointerToFree - HEADER_SIZE;

    int initializationCheck = ((int *)memoryStart)[1];
    if(initializationCheck == 0) {
        if(DEBUG)
            printf("First time malloc or free is called initializationCheck is %d\n", initializationCheck);
        setHeader(memoryStart, 1, MEMORY_SIZE_BYTES);
    }

    if (DEBUG){
        printf("\nfree called with %p pointer at %s %d \n ", pointerToFree, sourceFile, sourceLine);
        util();
    }

    char* previousBlockPointer = NULL;
    int blockPosition = 0;
    while(1) {
        if (blockPosition >= MEMORY_SIZE_BYTES){
            break;
        }
        if (pointerToFree == &memoryStart[blockPosition]) {
            break;
        }
        else {
            previousBlockPointer = &memoryStart[blockPosition];
            blockPosition += ((int*) memoryStart)[(blockPosition/INTEGER_SIZE) + 1];
        }        
    }

    if (blockPosition >= MEMORY_SIZE_BYTES) {
        printf("ERROR: ptr not found at header %s %d \n",  sourceFile, sourceLine);
        return;
    }
    if (((int*)pointerToFree)[0] == 0) {
        ((int*)pointerToFree)[0] = 1;
    } else {
        printf("ERROR: trying to free already freed memory at %p at %s %d \n", pointerToFree, sourceFile, sourceLine);
        return;
    }
    
    coa(previousBlockPointer, pointerToFree);
}


void util() { 
    int heapIndex = 0;

    printf("\n  printing out blocks:\n");

    while(1) {

        if (heapIndex >= MEMORY_SIZE_BYTES){
            break;
        }
        else {
            if (((int *)memoryStart)[(heapIndex/INTEGER_SIZE)]){
                printf("        open block of size %d at %d bytes in \n", ((int *)memoryStart)[(heapIndex/INTEGER_SIZE) + 1], heapIndex);
            }
            else{
                printf("        in use block of size %d at %d bytes in\n", ((int *)memoryStart)[(heapIndex/INTEGER_SIZE) + 1], heapIndex);
            }
            heapIndex = heapIndex + ((int*) memoryStart)[(heapIndex/INTEGER_SIZE) + 1];
            
        }        
    }
}
