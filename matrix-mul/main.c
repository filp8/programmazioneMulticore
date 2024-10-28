#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <mpi.h>

#include "utils/macros.h"
#include "utils/logging.h"
#define RANDOM_IMPLEMENTATION
#include "utils/random.h"

#include "matrix.h"

typedef struct {
    size_t i;
    size_t j;
} Point;

typedef struct {
    Point *data;
    size_t length;
    size_t capacity;
} DA_Points;

typedef struct {
    int *data;
    size_t length;
    size_t capacity;
} Local_Result;

DA_Points distribuite_cols(
    size_t matrix_order,
    int my_rank, int comm_size,
    int *recvcounts, int *displs
) {
    if(recvcounts != NULL) {
        memset(recvcounts, 0, comm_size*sizeof(*recvcounts));
    }
    if(displs != NULL) {
        memset(displs, -1, comm_size*sizeof(*displs));
    }
    DA_Points result = {0};
    size_t total_len = matrix_order*matrix_order; // soluzioni totali da calcolare
    size_t solution_for_rank = total_len/comm_size;
    size_t remaining_solutions = total_len % comm_size;
    size_t distribuited_solutions = 0;
    int current_rank = comm_size - 1;
    size_t i = 0;
    size_t j = 0;
    for(i=0; i < matrix_order; i++) {
        for(j=0; j < matrix_order; j++) {
            if(displs != NULL && displs[current_rank] == -1) {
                displs[current_rank] = i*matrix_order + j;
            }
            if(recvcounts != NULL)
                recvcounts[current_rank]++;
            if(current_rank == my_rank) {
                Point to_assign = {
                    .i = i,
                    .j = j,
                };
                append(&result, to_assign);
            }
            distribuited_solutions++;
            if(distribuited_solutions == solution_for_rank) {
                if((size_t)current_rank > remaining_solutions || remaining_solutions == 0) {
                    distribuited_solutions = 0;
                    total_len -= solution_for_rank;
                    current_rank = (current_rank - 1 + comm_size) % comm_size;
                } else {
                    distribuited_solutions--;
                    remaining_solutions--;
                    total_len--;
                }
            }
            
            fatal_if(total_len == 0 && i < matrix_order - 1 && j < matrix_order - 1,
                "ended solutions, but points aren't finished");
        }
    }

    fatal_if(total_len != 0,
        "Not all solutions was distribuited,remaining: %lu", total_len); //TODO: typo

    return result;
}

int main(int argc, char **argv) {
    // in argv ci passiamo l'ordine della matrice
    Control(MPI_Init(&argc, &argv));

    Control(MPI_Barrier(MPI_COMM_WORLD));
    double start = MPI_Wtime();

#if DEBUG
    setvbuf(stderr, NULL, _IONBF, 0);
    set_log_level(LOG_DEBUG);
#endif // DEBUG

    size_t order = parse_order_matrix(argc, argv);
    size_t total_len = order*order;

    int rank, size;

    Control(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    Control(MPI_Comm_size(MPI_COMM_WORLD, &size));

    int *mtx1 = NULL;
    int *mtx2 = NULL;


    if(rank == size - 1) {
        init_random();
        mtx1 = generate_matrix(order);
        mtx2 = generate_matrix(order);
        #if DEBUG
            eprintf("Prima matrice:\n");
            eprintMatrix(mtx1, order);
            eprintf("Seconda matrice:\n");
            eprintMatrix(mtx2, order);
        #endif // DEBUG
        reverse_matrix(mtx2, order);
        #if DEBUG
            eprintf("Matrice trasposta:\n");
            eprintMatrix(mtx2, order);
        #endif // DEBUG
    } else {
        mtx1 = (int*)malloc(total_len*sizeof(*mtx1));
        mtx2 = (int*)malloc(total_len*sizeof(*mtx2));
    }

    Control(MPI_Bcast(
        mtx1, total_len, MPI_INT,
        size - 1, MPI_COMM_WORLD
    ));

    Control(MPI_Bcast(
        mtx2, total_len, MPI_INT,
        size - 1, MPI_COMM_WORLD
    ));

    int *recvcounts = NULL;
    int *displs = NULL;

    if(rank == 0) {
        recvcounts = malloc(size*sizeof(*recvcounts));
        displs = malloc(size*sizeof(*displs));
    }

    DA_Points my_points = distribuite_cols(order, rank, size, recvcounts, displs);

    //log_debug("Rank %d, My total points are: %lu", rank, my_points.length);

    Local_Result lr = {0};

    foreach(Point ,point, &my_points) {
        int *row = mtx1 + point->i*order;
        int *col = mtx2 + point->j*order;
        int result = dot_product(row, col, order);
        append(&lr, result);
        log_debug("Rank %d, c_{%lu,%lu} = %d", rank, point->i, point->j, result);
    }

    int *result_matrix = NULL;

    if(rank == 0) {
        result_matrix = malloc(total_len*sizeof(*result_matrix));
    }

    Control(MPI_Gatherv(
        lr.data, lr.length, MPI_INT,
        result_matrix, recvcounts, displs,
        MPI_INT, 0, MPI_COMM_WORLD
    ));

    Control(MPI_Barrier(MPI_COMM_WORLD));
    double end = MPI_Wtime();
    double elapsed = end - start;
    double result = 0;
    Control(MPI_Reduce(&elapsed, &result, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD));

    #if 0
    if(rank == 0) {
        eprintf("Matrice risultante:\n");
        eprintMatrix(result_matrix, order);
        eprintf("Tempo messo: %lf", result);
    }
    #endif // DEBUG

    if(rank == 0)
        eprintf("Tempo messo: %lf\n", result); 

    free(mtx1);
    free(my_points.data);
    free(mtx2);
    free(lr.data);
    if(result_matrix != NULL) free(result_matrix);
    Control(MPI_Finalize());
    return 0;
}