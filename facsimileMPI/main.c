#include <stdio.h>

#include <mpi.h>

#include "utils/macros.h"
#include "utils/logging.h"

int main(int argc, char **argv) {
    Control(MPI_Init(&argc, &argv));

    int rank, size;

    Control(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    Control(MPI_Comm_size(MPI_COMM_WORLD, &size));

    printf("Hello World from %d rank!\n", rank);

    Control(MPI_Finalize());
    return 0;
}
