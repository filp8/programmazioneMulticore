#include <stdio.h>
#include <stdlib.h>

#include <time.h>

#include <mpi.h>

#include "utils/macros.h"
#define LOGGING_IMPLEMENTATION
#include "utils/logging.h"


size_t parse_order(int argc, char **argv) {
    char *program_name = argv[0];
    fatal_if(argc != 2, "numero sbagliato di parametri");
    char *order = argv[1];
    char *endptr;
    size_t result = strtol(order, &endptr, 10);
    fatal_if(*endptr != '\0', "la stringa è sbagliata: %s", order);
    return result;
}

int *generate_matrix(size_t order) {
    size_t tot_length = order*order;
    int *result = malloc(tot_length*sizeof(int));
    for(size_t i = 0; i < tot_length; i++) {
        result[i] = rand() % 10;
    }
    return result;
}

int main(int argc, char **argv) {
    // in argv ci passiamo l'ordine della matrice
    Control(MPI_Init(&argc, &argv));

    size_t order = parse_order(argc, argv);

    int rank, size;

    Control(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    Control(MPI_Comm_size(MPI_COMM_WORLD, &size));

    if(rank == size - 1) {
        srand(time(NULL));
        int *mtx1 = generate_matrix(order);
        int *mtx2 = generate_matrix(order);

        

        free(mtx1);
        free(mtx2);
    }

    size_t vecs_for_rank = order/size;
    size_t r = order % size;



    Control(MPI_Finalize());
    return 0;
}