# Detect OS for OpenMP flags
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
    # macOS with Homebrew libomp
    CC=clang
    CFLAGS=-O2 -Xpreprocessor -fopenmp -std=c11 -Wall -I/opt/homebrew/opt/libomp/include
    LDFLAGS=-lm -L/opt/homebrew/opt/libomp/lib -lomp
else
    # Linux
    CC=gcc
    CFLAGS=-O2 -fopenmp -std=c11 -Wall
    LDFLAGS=-lm
endif

all: pi_openmp

pi_openmp: pi_openmp.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f pi_openmp

