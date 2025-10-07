memtest: memtest.o mymalloc.o
	gcc memtest.o mymalloc.o -o memtest

memgrind.o: memtest.c mymalloc.h
	gcc -c memtest.c

mymalloc.o: mymalloc.c mymalloc.h
	gcc -c mymalloc.c

clean:
	rm -f memtest *.o