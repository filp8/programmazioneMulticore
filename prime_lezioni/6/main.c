#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <time.h>

#include <mpi.h>

#define LOGGING_IMPLEMENTATION
#include "utils/logging.h"

// utilizzamo il metodo di montecarlo per approssimare pi greco
// pigreco = (#(p nel cerchio)*4)/(#p nel quadrato)

bool fall_in_circle(void) {
    double x = (double)rand() / ((double)RAND_MAX) * 2 - 1;
    double y = (double)rand() / ((double)RAND_MAX) * 2 - 1;

    return x*x + y*y <= 1;
}

#define ITERATIONS (100*100*100*100)

int main(void) {

    int size, rank;

    Control(MPI_Init(NULL, NULL));

    Control(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    Control(MPI_Comm_size(MPI_COMM_WORLD, &size));

    srand(time(NULL)*(rank+1));
    
    size_t in_circle = 0;
    size_t i;
    double start_time;
    if(rank == 0)
        start_time = MPI_Wtime();
    
    for(i=0; i < ITERATIONS/(size_t)size; i++) {
        if(fall_in_circle()) {
            in_circle++;
        }
    }


    size_t total_in_circle;
    Control(MPI_Reduce(
        &in_circle, &total_in_circle, 1
        , MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD
    ));

    if(rank == 0) {
        double end_time = MPI_Wtime();
        double pi = ((double)total_in_circle*4)/ITERATIONS;
        printf("Pi greco vale: %lf\ntempo di esecuzione: %lf\n", pi, end_time - start_time);
    }

    Control(MPI_Finalize());
    return 0;
}