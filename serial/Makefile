CC=gcc
CFLAGS=-O2 -std=c11 -Wall
LDFLAGS=-lm
all: pi_serial

pi_serial: pi_serial.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f pi_serial

