CC     := gcc
CFLAGS := -O2 -Wall -Wextra -std=c11

all: fat

fat: fat.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f fat kwufs.img
