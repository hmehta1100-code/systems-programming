Systems Programming: Project mylittlemalloc() by Vaishvi Patel and Himani Mehta

Outline:

    We were given the task of coming up with our own dummy versions of malloc() and free(). It must be capable of doing the following which we implemented in our unique ways:

    memtest.c
    - main: covers memory allocation (allocating with objsize and returning null if unsuccessful), memory initialization (after allocating each chunk has distinct sizes), memory deallocation (making sure each block has the correct value, incrementing errors if it does not, and if leak = 0 then memory is freed), program will also print the number of incorrect bytes, if any

    mymalloc.c: 
    - memory of 4096 bytes
    - struct chunk: stores the size of the chunk as well as whether or not the chunk is allocated or not
    - malloc: #define malloc(X) mymalloc(X, __FILE__, __LINE__), memory allocation function that checks initialization, adjusts size, and iterates through the heap to find free spaces (keeps going)
        if there is not enough space mymalloc will give an error reporting the size that was attemp
    - free: #define free(X) myfree(X, __FILE__, __LINE__), memory deallocation function that initializes if not already, checks if the pointer passed is appropriate, marks pointer to given chunk as free, and coalesces free chunks
        for coalescing, the function iterates from beginning of memory to the pointer given to merge any previous chunks that may be free and from the pointer given to the end of memory to combine free chunks
        chunks were combined by adding the sizes of the two chunks and marking the one that is first in memory as free to account for the full chunk
    - Various getters used to help us get the start of a chunk, the next chunk, and hold all its data unique to each one. These are especially useful when traversing through the heap.
    - getChunkStart: takes the pointer currently pointing to the start of the payload portion of the chunk to the start of the metadata by subtracting the size of the struct chunk
    - getNextChunk: takes a pointer to the next chunk in the heap by adding the size of the current chunk to the pointer to get to the end of the payload and adding the size of the struct of the chunk to get to the start of the payload for the next chunk
    - initialize: initialize the entire heap to be one free chunk, we know the heap ends with the last chunk being -1 as we used unsigned ints, using a static int to check initialization and calling leak detection atexit() to detect possible leaks
    - detectLeak: will go through the newly initialized heap and check if there are any chunks already allocated, get the size of those chunks, and report the size of the chunk leaked as well as the number of chunks leaked

Detectable Errors:

    Inappropriate Pointers:
    1. calling free with an address not obtained from malloc
    2. calling free with an address not at the start of a chunk
    3. calling free for a second time on the same pointer

    Note: After printing the error message, free() should terminate the process with exit status 2, using the call exit(2).

test: allocate 4096-x bytes






