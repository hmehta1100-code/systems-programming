#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#ifndef REALMALLOC
#include "mymalloc.h"
#endif

#ifndef LEAK
#define LEAK 0
#endif

#define MEMSIZE 4096
#define HEADERSIZE 8
#define OBJECTS 64
#define OBJSIZE (MEMSIZE / OBJECTS - HEADERSIZE)

int main(int argc, char **argv)
{
	//initalize of size objects
	char *obj[OBJECTS];
	int i, j, errors = 0;
	
	// fill memory with objects
	for (i = 0; i < OBJECTS; i++) {
		obj[i] = malloc(OBJSIZE); //being filled using call to malloc
		if (obj[i] == NULL) {
		    printf("Unable to allocate object %d\n", i); //will print if less than what is asked is allocated
		    exit(1); //also helpful for debugging
		}
	}
	
	//fill each object with distinct bytes
	for (i = 0; i < OBJECTS; i++) {
		memset(obj[i], i, OBJSIZE);
	}
	
	//check that all objects contain the correct bytes
	for (i = 0; i < OBJECTS; i++) {
		for (j = 0; j < OBJSIZE; j++) {
			if (obj[i][j] != i) {
				errors++;
				printf("Object %d byte %d incorrect: %d\n", i, j, obj[i][j]);
				//if an error is found, it prints a message indicating which object and byte were incorrect 
				//different from expected value of i
			}
		}
	}

	//free all objects
	if (!LEAK) {
	    for (i = 0; i < OBJECTS; i++) {
		free(obj[i]);
	    }
	}
	
	//reports final number of incorrect bytes, if any
	printf("%d incorrect bytes\n", errors);
	
	return EXIT_SUCCESS;
}
