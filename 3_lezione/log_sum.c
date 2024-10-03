#include <stdlib.h>
#include <stdio.h>

#include <time.h>
#include <assert.h>
#include <math.h>

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

    if(r != MPI_SUCCESS) {
        log_error("Error starting MPI program. Terminating");
        MPI_Abort(MPI_COMM_WORLD, r);
    }

    //set_log_level(LOG_DEBUG);

    int size, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    rank++;

    if(!IS_POW2(size)) {
        log_error("Size must be power of 2");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    srand(time(NULL));
    int *to_sum = generate_random_vector(LENGTH/size);

    size_t result = 0;

    for(int i = 0; i < LENGTH/size ; i++) {
        result += to_sum[i];
    }
    free(to_sum);

    log_debug("%d:Done the sum, partial result: %ld",rank, result);
    int i = 0;
    bool mandare = (rank) % 2 == 1;
    while(i < log2(size)) {
        if((rank) % (size_t)pow(2,i+1) == 0) {
            int source = (rank - pow(2,i));
            size_t other_solution;
            // ricevere
            log_debug("%d receiving from %d", rank , source);
            MPI_Recv(&other_solution, 1, MPI_LONG_LONG, source - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            log_debug("%d preso da %d", rank , source);

            result += other_solution;

            mandare = true;
        } else {
            if(mandare) {
                // invia
                int dest = (rank + pow(2,i));
                log_debug("%d sending to %d", rank , dest);
                MPI_Send(&result, 1, MPI_LONG_LONG, dest - 1, 0, MPI_COMM_WORLD);
                mandare = false;
            }
        }
        i++;
    }

    if(rank == size) {
        printf("result = %ld\n", result);
    }

    MPI_Finalize();
}