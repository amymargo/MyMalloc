CC = gcc
CFLAGS = -std=c99 -Wall -fsanitize=address,undefined -g

all: stressprogram dstressprogram generaltestprogram 


grind:stressprogram
	./stressprogram

gen: generaltestprogram
	./generaltestprogram

dgrind:dstressprogram
	./dstressprogram

dgen: dgeneraltestprogram
	./dgeneraltestprogram

stressprogram: memgrind.o mymalloc.o
	$(CC) $(CFLAGS)  $^ -o $@

dstressprogram: memgrind.o mymallocdebug.o
	$(CC) $(CFLAGS)  $^ -o $@

generaltestprogram: genmemtest.o mymalloc.o
	$(CC) $(CFLAGS)  $^ -o $@

dgeneraltestprogram: genmemtest.o mymallocdebug.o
	$(CC) $(CFLAGS)  $^ -o $@

mymalloc.o memgrind.o genmemtest: mymalloc.h

mymallocdebug.o: mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -DDEBUG -c $< -o $@

clean: 
	rm -f *.o testprogram stressprogram dstressprogram generaltestprogram


