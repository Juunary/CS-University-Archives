CC=gcc
CFLAGS=-O2 -pthread

all: numgen fork thread
numgen: numgen.c
	$(CC) -O2 $< -o $@
fork: fork.c
	$(CC) -O2 $< -o $@
thread: thread.c
	$(CC) $(CFLAGS) thread.c -o thread
clean:
	rm -f numgen fork thread temp.txt
