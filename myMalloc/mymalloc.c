#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "mymalloc.h"

//DONE 
#define SIZE 4096  //total memory size
#define ROUNDUP8(x) (((x) + 7) & (-8))  // 8 - byte alignment

char memory[SIZE]; // char heap
char *memoryEnd = memory + SIZE;    // end of the memory array (start address + 4096 bytes
static int memInitialized = 0;  // condition to check if heap is initialized

//metadata for memory chunks
struct chunk {
    size_t size;  //size of the chunk (excluding metadata)
    bool isFree;  //boolean value used instead of int to check if chunk is free
};

// leak detector for end of initialization
void detectLeak() {

    // local var
    struct chunk *ptr = (struct chunk*)memory;
    size_t leakedBytes = 0;  // 
    int leakedChunks = 0;

    //compare before current reaches the end of memory
    while(ptr < (struct chunk*)memoryEnd){

        //checks if not free and size (to record leak detection)
        if((ptr->isFree == false) && (ptr->size > 0)){ 
            leakedBytes += ptr->size;
            leakedChunks++;
        }
        ptr = (struct chunk*)((char*)ptr + sizeof(struct chunk) + ptr->size);   // iterate through heap
    }

    // if leaked
    if(leakedChunks > 0) {
        fprintf(stderr, "mymalloc: %zu bytes leaked in %d objects.\n", leakedBytes, leakedChunks);
    }
}

//initialize heap
void initializeHeap() {
    if (memInitialized) return;  //prevent reinitialization 

    memset(memory, 0, SIZE); //call to memory using memory setting function
    struct chunk *firstChunk = (struct chunk*)memory;
    firstChunk->size = SIZE - sizeof(struct chunk); //adjusting the size of the chunk
    firstChunk->isFree = true; //marking the chunk as free

    memInitialized = 1;
    atexit(detectLeak); //leak detection on exit
}

//get next chunk in memory
//calculates the next chunk in memory by moving the pointer ptr by the size of the current chunk's metadata
//plus the size of the data portion of the chunk
void *getNextChunk(void *ptr) {
    return (char *)ptr + sizeof(struct chunk) + ((struct chunk *)ptr)->size;
}

//get chunk metadata from user pointer
//moving the pointer back by the size of the chunk's metadata 
struct chunk *getChunkStart(void *ptr) {
    return (struct chunk *)((char *)ptr - sizeof(struct chunk));
}

//malloc function
void *mymalloc(size_t size, char *file, int line) {
    if (memInitialized == 0) initializeHeap();
    
    size = ROUNDUP8(size);  // 8-byte alignment
    size -= sizeof(struct chunk*); //subtracts the size of the chunk's metadata (the header)

    struct chunk *current = (struct chunk *)memory;

    while ((char *)current < memoryEnd) {
        if ((current->isFree) && (current->size >= size)) {
            size_t remainingSize = current->size - size - sizeof(struct chunk);

            // if space to create a new chunk split according to size
            if (remainingSize >= sizeof(struct chunk) + 8) {  
                struct chunk *newChunk = (struct chunk *)((char *)current + sizeof(struct chunk) + size);
                newChunk->size = remainingSize;
                newChunk->isFree = true;

                current->size = size;
            }
            
            // set allocated chunk as not free
            current->isFree = false;
            return (char *)current + sizeof(struct chunk);  // return void pointer to allocated chunk
        }
        current = getNextChunk(current);
    }

    fprintf(stderr, "malloc: Unable to allocate %d bytes (%s:%d)\n", size, file, line);
    return NULL;  // out of memory
}

//free function
void myfree(void *ptr, char *file, int line) {

    if(memInitialized == 0) initializeHeap();
    
    struct chunk *mem = getChunkStart(ptr);

    //DETECTABLE ERRORS - DONE
    //check if ptr is obtained from malloc()
    if ((mem < (struct chunk*)memory) || (mem >= (struct chunk*)memoryEnd)) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }

    //check if ptr is at the start of the chunk (i.e., if pointer is valid)
    if ((char*)ptr != (char*)mem + sizeof(struct chunk)) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }

    //check if same pointer is freed twice
    if (mem->isFree) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }

    mem->isFree = true;

    //merge with next chunk if free
    struct chunk *next = getNextChunk(mem);
    while ((char *)next < memoryEnd && next->isFree) {
        mem->size += sizeof(struct chunk) + next->size; // add chunk sizes together
        next = getNextChunk(mem);   // continue checking next chunks
    } 

    //mere with previous chunk if free
    struct chunk *prev = (struct chunk *)memory;    // beginning of memory
    while ((char *)prev < (char *)mem) {    // goes from beginning to start of chunk
        struct chunk *next = getNextChunk(prev);
        if (next == mem && prev->isFree) {
            prev->size += sizeof(struct chunk) + mem->size; // add chunk sizes together
            return;
        }
        prev = next;    // iterate to next chunk
    }
}