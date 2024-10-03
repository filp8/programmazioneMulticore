#include <stdlib.h>
#include <stdio.h>

#include <time.h>
#include <assert.h>

#include <mpi.h>

#include "utils/macros.h"
#define LOGGING_IMPLEMENTATION
#include "utils/logging.h"


int *generate_random_vector(size_t n_elements) {
    int *result = malloc(n_elements*sizeof(*result));
    assert(result != NULL);
    for(size_t i = 0; i < n_elements; i++) {
        result[i] = rand();
    }
    return result;
}

#define LENGTH 1024*1024*1024

int main(void) {
    int r = MPI_Init(NULL,NULL);

    //set_log_level(LOG_DEBUG);

    if(r != MPI_SUCCESS) {
        log_error("Error starting MPI program. Terminating");
        MPI_Abort(MPI_COMM_WORLD, r);
    }

    int size, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(time(NULL));
    int *to_sum = generate_random_vector(LENGTH/size);

    size_t result = 0;

    for(int i = 0; i < LENGTH/size ; i++) {
        result += to_sum[i];
    }
    free(to_sum);
    if(rank != size - 1) {
        size_t other_solution;
        log_debug("%d:Done the sum, partial result: %ld",rank, result);
        MPI_Recv(&other_solution, 1, MPI_LONG_LONG, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        result += other_solution;
        log_debug("%d:Added other solution, result: %ld",rank, result);
    }
    if(rank != 0) {
        MPI_Send(&result, 1, MPI_LONG_LONG, rank - 1, 0, MPI_COMM_WORLD);
    } else {
        printf("result = %ld\n", result);
    }
    MPI_Finalize();
}