#include <stdio.h>

#include <mpi.h>

#include "utils/macros.h"
#include "utils/random.h"
#include "utils/matrix.h"
#include "utils/logging.h"

int main(int argc, char **argv) {
    Control(MPI_Init(&argc, &argv));

    size_t col ;
    size_t rig ;
    size_t S;


    if (argc==4){
        col = atol(argv[1]);
        rig = atol(argv[2]);
        S = atol(argv[3]);
        fatal_if((col == 0 || rig == 0 || S == 0),"un argomento non e un numero o e zero");
    }else{
        log_fatal("numero di argomenti sbagliato");
    }


    

    int rank, size;

    Control(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    Control(MPI_Comm_size(MPI_COMM_WORLD, &size));

    int *mat = NULL;

    if (rank == 0){
        init_random();
        mat = generate_matrix_n_m(rig,col,-5,5);

    }else{
        mat = malloc((rig*col)*sizeof(int));
    };

    Control(MPI_Bcast(mat,rig*col,MPI_INT,0,MPI_COMM_WORLD));
    


    Control(MPI_Finalize());
    return 0;
}