/* pi_openmp.c
   Compile: make (in openmp/)  or: gcc -fopenmp pi_openmp.c -O2 -lm
   Run example: ./pi_openmp 100000000 8
   Arguments: <samples> <threads>
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>

int main(int argc, char *argv[]) {
    long long n = 100000000LL;
    int threads = 4;
    if (argc > 1) n = atoll(argv[1]);
    if (argc > 2) threads = atoi(argv[2]);

    omp_set_num_threads(threads);
    long long global_count = 0;
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    #pragma omp parallel
    {
        unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)(omp_get_thread_num()*0x9e3779b1);
        long long local_count = 0;
        long long chunk = n / omp_get_num_threads();
        long long start = omp_get_thread_num() * chunk;
        long long end = (omp_get_thread_num() == omp_get_num_threads()-1) ? n : start + chunk;

        for (long long i = start; i < end; ++i) {
            double x = rand_r(&seed) / (RAND_MAX + 1.0);
            double y = rand_r(&seed) / (RAND_MAX + 1.0);
            if (x*x + y*y <= 1.0) ++local_count;
        }

        #pragma omp atomic
        global_count += local_count;
    }

    clock_gettime(CLOCK_MONOTONIC, &t1);
    double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
    double pi = 4.0 * (double)global_count / (double)n;

    printf("MODE,OPENMP\n");
    printf("SAMPLES,%lld\n", n);
    printf("THREADS,%d\n", threads);
    printf("INSIDE,%lld\n", global_count);
    printf("PI,%.10f\n", pi);
    printf("ERROR,%.10f\n", fabs(pi - M_PI));
    printf("TIME,%.6f\n", elapsed);

    return 0;
}

