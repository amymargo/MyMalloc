#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

//#define REALMALLOC 1

// Compile with -DREALMALLOC to use the real malloc() instead of mymalloc()
#ifndef REALMALLOC
#include "mymalloc.h"
#endif
#define MEMSIZE 4096
#define HEADERSIZE 8

#define ALIGNMENT 8

#define NUM_OBJECTS 8
#define OBJECT_SIZE 500


void testMalloc(){
    printf("Testing malloc(): \n");
    char *allocatedObjects[NUM_OBJECTS];
    int objectIndex, byteIndex = 0;

    // Allocate memory for objects
    for (objectIndex = 0; objectIndex < NUM_OBJECTS; objectIndex++) {
        allocatedObjects[objectIndex] = malloc(OBJECT_SIZE);
    }

    // Initialize each object with distinct bytes
    for (objectIndex = 0; objectIndex < NUM_OBJECTS; objectIndex++) {
        memset(allocatedObjects[objectIndex], objectIndex, OBJECT_SIZE);
    }

    // Verify each object contains the correct bytes
    for (objectIndex = 0; objectIndex < NUM_OBJECTS; objectIndex++) {
        for (byteIndex = 0; byteIndex < OBJECT_SIZE; byteIndex++) {
            if (allocatedObjects[objectIndex][byteIndex] != objectIndex) {
                printf("test failed: malloc");
                exit(1);
            }
        }
    }

    // Free all objects
    for (objectIndex = 0; objectIndex < NUM_OBJECTS; objectIndex++) {
        free(allocatedObjects[objectIndex]);
    }
    printf("TEST SUCCEEDED\n");
}

void testAlign(){
    printf("Testing align(): \n");

    int allocationIndex = 0;
    char* alignmentTestObjects[ALIGNMENT];
    for (allocationIndex = 1; allocationIndex < ALIGNMENT; allocationIndex++){
        alignmentTestObjects[allocationIndex] = malloc(allocationIndex);
    }
    for (allocationIndex = 1; allocationIndex < ALIGNMENT; allocationIndex++){
        if (((uintptr_t)alignmentTestObjects[allocationIndex]) % 8 != 0){
            printf("test failed: align");
            exit(1);
        }

    }
    for (allocationIndex = 1; allocationIndex < ALIGNMENT; allocationIndex++){
        free(alignmentTestObjects[allocationIndex]);
    }
    printf("TEST SUCCEEDED\n");

}

void testCoa(){
    printf("Testing coa(): \n");
    char* coalesceTestObjects[8];
    int coalesceIndex = 0;
    for (coalesceIndex = 1; coalesceIndex < ALIGNMENT; coalesceIndex++){
        coalesceTestObjects[coalesceIndex] = malloc(coalesceIndex);
    }

    // Free in specific order to test coalescing under various conditions
    free(coalesceTestObjects[7]);
    free(coalesceTestObjects[2]);
    free(coalesceTestObjects[1]);
    free(coalesceTestObjects[6]);
    free(coalesceTestObjects[5]);
    free(coalesceTestObjects[3]);
    free(coalesceTestObjects[4]);

    // Test successful coalescing by attempting a large allocation
    char* largeAllocationTest = malloc(4088);
    free(largeAllocationTest);
    printf("TEST SUCCEEDED\n");

}

void freeNonMallocAddress() {
    printf("Testing free on Non-malloc Address, Error expected: \n");
    int stackVariable = 2;
    free(&stackVariable);
}

void freeOnMiddleAddress() {
    printf("Testing free on Middle Address, Error expected: \n");

    int* largeArray = malloc(sizeof(int) * 100);
    free(largeArray + 1);
}

void doubleFree() {
    printf("Testing double free, Error expected: \n");

    char* doubleFreeTest = malloc(16);
    free(doubleFreeTest);
    free(doubleFreeTest);
}

int main(int argc, char **argv)
{
    testMalloc();
    testAlign();
    testCoa();
    //no need to have a test just for free since free is called in all other tests
    freeNonMallocAddress();
    freeOnMiddleAddress();
    doubleFree();
}
