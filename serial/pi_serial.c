/* pi_serial.c
   Serial Monte Carlo estimation of Pi
   Compile: make (in serial/)  or: gcc pi_serial.c -O2 -o pi_serial -lm
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int main(int argc, char *argv[]) {
    long long n = 10000000LL; /* default number of samples */
    if (argc > 1) n = atoll(argv[1]);

    long long count = 0;
    double x, y;
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    srand((unsigned)time(NULL) ^ 0xC0FFEE);

    for (long long i = 0; i < n; ++i) {
        x = rand() / (RAND_MAX + 1.0);
        y = rand() / (RAND_MAX + 1.0);
        if (x * x + y * y <= 1.0) ++count;
    }

    clock_gettime(CLOCK_MONOTONIC, &t1);
    double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

    double pi = 4.0 * (double)count / (double)n;
    printf("MODE,SERIAL\n");
    printf("SAMPLES,%lld\n", n);
    printf("INSIDE,%lld\n", count);
    printf("PI,%.10f\n", pi);
    printf("ERROR,%.10f\n", fabs(pi - M_PI));
    printf("TIME,%.6f\n", elapsed);

    return 0;
}

