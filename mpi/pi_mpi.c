/* pi_mpi.c
   Compile: make (in mpi/) or: mpicc pi_mpi.c -O2 -lm
   Run: mpirun -np 4 ./pi_mpi <total_samples>
*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <time.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long long total_n = 100000000LL;
    if (argc > 1) total_n = atoll(argv[1]);

    long long local_n = total_n / size;
    if (rank == size - 1) local_n += total_n % size; // last rank takes remainder

    unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)(rank * 0x9e3779b1);
    long long local_count = 0;

    double t0 = MPI_Wtime();
    for (long long i = 0; i < local_n; ++i) {
        double x = rand_r(&seed) / (RAND_MAX + 1.0);
        double y = rand_r(&seed) / (RAND_MAX + 1.0);
        if (x*x + y*y <= 1.0) ++local_count;
    }
    double t1 = MPI_Wtime();
    double local_time = t1 - t0;

    long long global_count;
    MPI_Reduce(&local_count, &global_count, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    double max_time;
    MPI_Reduce(&local_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        double pi = 4.0 * (double)global_count / (double)total_n;
        printf("MODE,MPI\n");
        printf("SAMPLES,%lld\n", total_n);
        printf("PROCESSES,%d\n", size);
        printf("INSIDE,%lld\n", global_count);
        printf("PI,%.10f\n", pi);
        printf("ERROR,%.10f\n", fabs(pi - M_PI));
        printf("TIME,%.6f\n", max_time); /* use max time across processes */
    }

    MPI_Finalize();
    return 0;
}

