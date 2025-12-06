// pi_cuda.cu
// Parallel Monte Carlo Simulation for Estimating Pi using CUDA

// For Colab: nvcc -O2 pi_cuda.cu -o pi_cuda -gencode arch=compute_75,code=sm_75
// Run: ./pi_cuda [samples] [blocks] [threads_per_block]

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cuda_runtime.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Xorshift32 RNG - fast PRNG suitable for GPU
__device__ unsigned int xorshift32(unsigned int *state) {
    unsigned int x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

// Convert to uniform float in [0, 1)
__device__ float rnd_uniform(unsigned int *state) {
    return (xorshift32(state) & 0x00FFFFFF) / 16777216.0f;
}

// Monte Carlo Pi kernel - each thread processes samples_per_thread samples
__global__ void pi_kernel(unsigned long long samples_per_thread, unsigned long long *d_count, unsigned int seed) {
    unsigned long long gid = blockIdx.x * blockDim.x + threadIdx.x;
    unsigned long long local_count = 0;
    
    // Initialize RNG state - CRITICAL: xorshift32 cannot have zero state
    // Using (gid + 1) ensures uniqueness, OR with 1u guarantees non-zero
    unsigned int st = (seed ^ (unsigned int)(gid + 1)) | 1u;
    
    // Warm up the RNG to improve randomness quality
    for (int w = 0; w < 10; ++w) {
        xorshift32(&st);
    }

    // Monte Carlo sampling: generate random (x,y) and check if inside unit circle
    for (unsigned long long i = 0; i < samples_per_thread; ++i) {
        float x = rnd_uniform(&st);
        float y = rnd_uniform(&st);
        if (x*x + y*y <= 1.0f) ++local_count;
    }
    
    // Atomically add local count to global counter
    if (local_count > 0) atomicAdd(d_count, local_count);
}

int main(int argc, char *argv[]) {
    // Default parameters
    unsigned long long total_samples = 100000000ULL;  // 10^8
    int blocks = 256;
    int threadsPerBlock = 256;

    // Parse command line arguments
    if (argc >= 2) total_samples = strtoull(argv[1], NULL, 10);
    if (argc >= 3) blocks = atoi(argv[2]);
    if (argc >= 4) threadsPerBlock = atoi(argv[3]);

    // Calculate work distribution
    unsigned long long total_threads = (unsigned long long)blocks * threadsPerBlock;
    unsigned long long samples_per_thread = total_samples / total_threads;
    if (samples_per_thread == 0) samples_per_thread = 1;
    unsigned long long effective_total = samples_per_thread * total_threads;

    // Allocate device memory for counter
    unsigned long long *d_count;
    cudaMalloc(&d_count, sizeof(unsigned long long));
    cudaMemset(d_count, 0, sizeof(unsigned long long));

    // Create CUDA events for timing
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start);

    // Use fixed seed for reproducibility
    unsigned int seed = 12345678u;
    
    // Launch kernel
    pi_kernel<<<blocks, threadsPerBlock>>>(samples_per_thread, d_count, seed);
    
    // Check for kernel launch errors
    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess) {
        fprintf(stderr, "Kernel launch failed: %s\n", cudaGetErrorString(err));
        return 1;
    }
    
    cudaDeviceSynchronize();

    // Stop timing
    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    float ms = 0;
    cudaEventElapsedTime(&ms, start, stop);

    // Copy result back to host
    unsigned long long h_count = 0;
    cudaMemcpy(&h_count, d_count, sizeof(unsigned long long), cudaMemcpyDeviceToHost);

    // Calculate Pi estimate
    double pi = 4.0 * (double)h_count / (double)effective_total;
    
    // Output in CSV format (consistent with other implementations)
    printf("MODE,CUDA\n");
    printf("SAMPLES,%llu\n", effective_total);
    printf("BLOCKS,%d\n", blocks);
    printf("THREADS_PER_BLOCK,%d\n", threadsPerBlock);
    printf("INSIDE,%llu\n", h_count);
    printf("PI,%.10f\n", pi);
    printf("ERROR,%.10f\n", fabs(pi - M_PI));
    printf("TIME,%.6f\n", ms / 1000.0);

    // Cleanup
    cudaFree(d_count);
    cudaEventDestroy(start);
    cudaEventDestroy(stop);
    
    return 0;
}
