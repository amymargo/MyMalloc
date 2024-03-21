#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/time.h>

// Compile with -DREALMALLOC to use the real malloc() instead of mymalloc()
#ifndef REALMALLOC
//#include "mymalloc.h"
#endif

#define MEMSIZE 4096
#define HEADERSIZE 8

#define NUMOBJECTS 120
#define NUMTIMES 50

#define NUMTESTS 5


int test1(){
    //1. malloc() and immediately free() a 1-byte object, 120 times.

    struct timeval tv;
    struct timeval tv2;
    gettimeofday(&tv,NULL);

    for (int i = 0; i < 120; i++) {
        void *ptr = malloc(1);
        if (ptr==NULL){
            printf("memory allocation failed in task 1\n");
            exit(1);
        }
        free(ptr);
    }

    gettimeofday(&tv2,NULL);
    long time_micro = (tv2.tv_sec - tv.tv_sec) * 1000000L + (tv2.tv_usec - tv.tv_usec);
    return time_micro;
}

int test2(){
    //2. Use malloc() to get 120 1-byte objects, storing the pointers in an array, then use free() to
    //deallocate the chunks.
    struct timeval tv;
    struct timeval tv2;
    gettimeofday(&tv,NULL);

    void *ptrs[120];

    for (int i = 0; i < 120; i++) {
        ptrs[i] = malloc(1);
        if (ptrs[i]==NULL){
            printf("memory allocation failed in task 2\n");
            exit(1);
        }
    }

    for (int i = 0; i < 120; i++) {
        free(ptrs[i]);
    }

    gettimeofday(&tv2,NULL);
    long time_micro = (tv2.tv_sec - tv.tv_sec) * 1000000L + (tv2.tv_usec - tv.tv_usec);
    return time_micro;
}

int test3(){
    //3. Create an array of 120 pointers. Repeatedly make a random choice between allocating a 1-byte
    //object and adding the pointer to the array and deallocating a previously allocated object (if
    //any), until you have allocated 120 times. Deallocate any remaining objects

    struct timeval tv;
    struct timeval tv2;
    gettimeofday(&tv,NULL);

    void *ptrs[120];
    int num_ptrs = 0;
    int allocations = 0;

    while (allocations<120) {
        int choice = rand() % 2;

        if (choice == 0) {
            //ptrs[num_ptrs++] = malloc(1);
            ptrs[num_ptrs] = malloc(1);
            if (ptrs[num_ptrs] == NULL) {
                printf("memory allocation failed in task3.\n");
                exit(1);
            }
            num_ptrs++;
            allocations++;
        } else if (choice == 1 && num_ptrs > 0) {
            free(ptrs[num_ptrs-1]);
            num_ptrs--;
        }
    }

    for (int i = 0; i < num_ptrs; i++) {
        free(ptrs[i]);
    }

    gettimeofday(&tv2,NULL);
    long time_micro = (tv2.tv_sec - tv.tv_sec) * 1000000L + (tv2.tv_usec - tv.tv_usec);
    return time_micro;
    }

int test4(){
    /*Allocate and free increasingly larger chunks of memory using malloc(), increasing the size by 100 each time, 
    until reaching the limit of 4088 bytes.*/

    struct timeval tv;
    struct timeval tv2;
    gettimeofday(&tv,NULL);

    void *ptrs[120];
    int size = 88;
    int i = 0;
    while (size<=4088){
        ptrs[i]=malloc(size);
        if (ptrs[i]==NULL){
            printf("memory allocation failed in task 4\n");
            exit(1);
        }
        free(ptrs[i]);
        size += 100;
        i++;
    }
    /*for (int j = 0; j<i; j++){
        free(ptrs[j]);
    }*/

    gettimeofday(&tv2,NULL);
    long time_micro = (tv2.tv_sec - tv.tv_sec) * 1000000L + (tv2.tv_usec - tv.tv_usec);
    return time_micro;
}

int test5(){
    //allocate and free one 4088 byte object, then allocate 2 2044 byte objects and free them, then 4 1022 byte objects, 
    //then 8 511 byte objects, 16 255 byte options, 32 127 byte objects, and 64 63 byte objects
    //(double the number of objects and half the number of bytes each time)

    struct timeval tv;
    struct timeval tv2;
    gettimeofday(&tv,NULL);

    void *ptrs[64];
    int num_objs = 1;
    int bytes=4088;
    
    while (num_objs<=64){
        for(int i = 0; i<num_objs; i++){
            //printf("%d", bytes);
            ptrs[i]=malloc(bytes);
            if (ptrs[i]==NULL){
            printf("memory allocation failed in task 5\n");
            exit(1);
        }
        }
        for(int i = 0; i<num_objs; i++){
            free(ptrs[i]);
        }
        num_objs=num_objs*2;
        bytes=bytes/2;
    }

    gettimeofday(&tv2,NULL);
    long time_micro = (tv2.tv_sec - tv.tv_sec) * 1000000L + (tv2.tv_usec - tv.tv_usec);
    return time_micro;
}



int main(int argc, char **argv)
{

    int j = 0;

    int times[NUMTESTS][NUMTIMES];

    for (j = 0;  j < NUMTIMES; j++){
        times[0][j] = test1();
        times[1][j] = test2();
        times[2][j] = test3();
        times[3][j] = test4();
        times[4][j] = test5();

    }

    int avg[NUMTESTS] = {0};
    int longest[NUMTESTS]  = {0};
    int shortest[NUMTESTS]  = {10000};

    int k = 0;

    for(j = 0; j < NUMTESTS; j++){
        for(k = 0; k < NUMTIMES; k++){
            avg[j] += times[j][k];
            if(longest[j] < times[j][k])
                longest[j] = times[j][k];

            if(shortest[j] > times[j][k])
                shortest[j] = times[j][k];
        }
    }

    for(j = 0; j < NUMTESTS; j++){
        printf("\nfor test %d\n", j+1);
        printf("avg time of %d microseconds\n", avg[j]/NUMTIMES);
        printf("longest time of %d microseconds\n", longest[j]);
        //printf("shortest time of %d microseconds\n", shortest[j]);
    }

    return EXIT_SUCCESS;

}
