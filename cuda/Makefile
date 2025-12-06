# CUDA Makefile
# For local: make
# For Google Colab: Just run these commands directly:
#   !nvcc -O2 pi_cuda.cu -o pi_cuda
#   !./pi_cuda

NVCC=nvcc
NVFLAGS=-O2

all: pi_cuda

pi_cuda: pi_cuda.cu
	$(NVCC) $(NVFLAGS) pi_cuda.cu -o pi_cuda

clean:
	rm -f pi_cuda

