#include <stdio.h>
#include <mpi.h>

#include "utils/macros.h"

int main(int argc, char **argv) {

    Control(MPI_Init(&argc, &argv));

    printf("Hello World\n");
    int rank,size;
    Control(MPI_Comm_rank(MPI_COMM_WORLD,&rank));
    Control(MPI_Comm_size(MPI_COMM_WORLD,&size));


    MPI_Request send_right, send_left;
    MPI_Request recv_right, recv_left;

    int right_next_rank = (rank + 1) % size;
    int left_next_rank = (rank - 1) %size;
    Control(MPI_Isend(
        &send_right, 1, MPI_INT, right_next_rank
        , 0, MPI_COMM_WORLD, &send_right
    ));

    Control(MPI_Isend(
        &send_left, 1, MPI_INT, left_next_rank
        , 0, MPI_COMM_WORLD, &send_left
    ));

    Control(MPI_Irecv(
        &recv_left, 1, MPI_INT, right_next_rank
        , 0, MPI_COMM_WORLD, &send_left
    ));

    Control(MPI_Irecv(
        &recv_right, 1, MPI_INT, left_next_rank
        , 0, MPI_COMM_WORLD, &send_left
    ));

    
    MPI_Request *res = {send_left, recv_right, send_right, recv_left};
    MPI_Status stat[ARRAY_LEN(res)] = {0};
    MPI_Waitall(4, res,stat);

    
    return 0;
}