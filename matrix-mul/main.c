#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <mpi.h>

#include "utils/macros.h"
#define LOGGING_IMPLEMENTATION
#include "utils/logging.h"

#include "matrix.h"

typedef struct {
    int i;
    int j;
} Point;

typedef struct {
    Point *data;
    size_t length;
    size_t capacity;
} DA_Points;

/*
    distribuisci i punti da calcolare, ma contando quante soluzioni vanno 
    per processo, senza contare il rimanente
*/
DA_Points distribuite_cols_main(
    int *mtx, size_t order,
    int my_rank, int comm_size,
    int *dest
) {
    DA_Points result = {0};
    size_t total_len = order*order; // soluzioni totali da calcolare
    size_t solution_for_rank = total_len/comm_size;
    // size_t remaining_solutions = total_len % comm_size;
    size_t distribuited_solutions = 0;
    int current_rank = 0;
    for(size_t i=0; i < order; i++) {
        for(size_t j=0; j < order; j++) {
            dest[current_rank];
            if(current_rank == my_rank) {
                Point to_assign = {
                    .i = i,
                    .j = j,
                };
                append(&result, to_assign);
            }
            distribuited_solutions++;
            if(distribuited_solutions == solution_for_rank) {
                distribuited_solutions = 0;
                current_rank = (current_rank + 1) % comm_size;
            }
            
            total_len -= solution_for_rank;
            fatal_if(total_len == 0 && i != order && j != order,
                "ended solutions, but points aren't finished");
        }
    }

    //TODO: calcolare quali punti i e j bisogna ancora calcolare
    DA_Points to_assign = {0};

    fatal_if(total_len != 0,
        "Not all solutions was distribuited"); //TODO: typo

    return result;
}

DA_Points distribuite_cols_pending(
    int *mtx, size_t order,
    int my_rank, int comm_size,
    int *dest
) {
    DA_Points result = {0};
    size_t total_len = order*order; // soluzioni totali da calcolare
    //size_t solution_for_rank = total_len/comm_size;
    size_t remaining_solutions = total_len % comm_size;


    size_t distribuited_solutions = 0;
    int current_rank = 0;

    for(int rank = 0; rank < comm_size; rank++) {

    }

    if(to_assign.data != NULL) free(to_assign.data);

    return result;
}
int main(int argc, char **argv) {
    // in argv ci passiamo l'ordine della matrice
    Control(MPI_Init(&argc, &argv));
    
    init_random();

    size_t order = parse_order(argc, argv);
    size_t total_len = order*order;

    int rank, size;

    Control(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    Control(MPI_Comm_size(MPI_COMM_WORLD, &size));

    int *mtx1 = NULL;
    int *mtx2 = NULL;


    if(rank == size - 1) {
        mtx1 = generate_matrix(order);
        mtx2 = generate_matrix(order);
        reverse_matrix(mtx2, order);
    } else {
        mtx1 = (int*)malloc(total_len*sizeof(*mtx1));
    }

    Control(MPI_Bcast(
        mtx1, total_len, MPI_INT,
        size - 1, MPI_COMM_WORLD
    ));

    int dest[size];
    memset(dest, 0, size*sizeof(*dest));
    DA_Points my_points = distribuite_cols(mtx2, order, rank, size, dest);

    log_debug("My total points are: %lu", my_points.length);

    free(mtx1);
    free(my_points.data);
    if(mtx2 != NULL) free(mtx2);
    Control(MPI_Finalize());
    return 0;
}