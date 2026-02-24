CC   := gcc
CFLAGS := -O2 -Wall -Wextra -std=c11

all: page_replacement_simulator

page_replacement_simulator: page_replacement_simulator.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f page_replacement_simulator
