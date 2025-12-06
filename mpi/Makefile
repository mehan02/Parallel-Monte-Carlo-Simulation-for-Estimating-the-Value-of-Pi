CC=mpicc
CFLAGS=-O2 -std=c11 -Wall
LDFLAGS=-lm
all: pi_mpi

pi_mpi: pi_mpi.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f pi_mpi

