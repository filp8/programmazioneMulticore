#include <stdio.h>

#include <mpi.h>

#define LOGGING_IMPLEMENTATION
#include "utils/logging.h"

int main(int argc, char **argv) {
    Control(MPI_Init(&argc, &argv));

    printf("Hello World!\n");

    Control(MPI_Finalize());
    return 0;
}