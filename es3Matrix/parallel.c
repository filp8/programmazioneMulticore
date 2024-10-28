#include <stdio.h>

#include <mpi.h>

#include "utils/macros.h"
#include "utils/random.h"
#include "utils/matrix.h"
#include "utils/logging.h"

typedef struct{ 
    int i;
    int j;
}Point;

void arrayDisposizioneRicezione(int size,size_t rig,size_t col,int arrOut[size]){
    size_t tot_point = rig*col;
    size_t points_for_rank = tot_point/size;
    size_t remaning_points = tot_point%size;

    for(int rank = 0;rank<size;rank++){
        if ((size_t)rank<remaning_points){
            arrOut[rank] = (int)(rank * (points_for_rank+1));
            
        }else{
            arrOut[rank] = (int)(remaning_points * (points_for_rank+1))+
                    ((rank-remaning_points)*points_for_rank);
            
        }  
    }
    
}void arrayNumElementiRicezione(int size,size_t rig,size_t col,int arrOut[size]){
    size_t tot_point = rig*col;
    size_t points_for_rank = tot_point/size;
    size_t remaning_points = tot_point%size;
    size_t inizio;
    size_t fine;

    for(int rank = 0;rank<size;rank++){
        if ((size_t)rank<remaning_points){
            inizio = (rank * (points_for_rank+1));
            fine = inizio + points_for_rank;
        }else{
            inizio = (remaning_points * (points_for_rank+1))+((rank-remaning_points)*points_for_rank);
            fine = inizio + points_for_rank-1;
        }
        arrOut[rank]=fine-inizio+1;  
    }
}

Point numToPoint(size_t n,size_t col){
    // (n-j)/rig = i
    Point p = {
        .i = n/col,
        .j = n%col
    };
    return p;
}

size_t pointToNum(Point p,size_t n_col){
    return (p.i*n_col)+p.j;
}

void arrayNumElementiInvio(
    int comm_size, int* sendcounts,
    size_t rows, size_t cols
) {
    
    int row_for_rank  = rows/comm_size;
    int row_remaining = rows%comm_size;

    for(int rank=0; rank<comm_size; rank++) {
        int to_sum = 0;
        if(rank < row_remaining) to_sum += cols;
        to_sum += row_for_rank*cols;
        sendcounts[rank] = to_sum;
    }
}

void arrayDisposizioneInvio(
    int comm_size, int* displs,int *sendcounts
) {
    
    displs[0]=0;
    for(int rank=1; rank<comm_size; rank++) {
        displs[rank] = sendcounts[rank-1] + displs[rank-1];
    }
}




int* ultima_riga(int* ricezione,int num_elementi,int col){
    return ricezione+num_elementi-col;
}


int main(int argc, char **argv) {
    Control(MPI_Init(&argc, &argv));

    size_t col = 0 ;
    size_t rig = 0 ;
    size_t S = 0 ;

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
        //init_random();
        srand(2476063558);
        mat = generate_random_matrix(rig,col,-5,5);
    }

    int num_elementi[size];
    int disposizione[size];
  
    arrayNumElementiInvio(size,num_elementi,rig,col);
    arrayDisposizioneInvio(size,disposizione,num_elementi);

    int *ricezione = malloc(num_elementi[rank]*sizeof(int));

    int rigaSopra[col];
    int rigaSotto[col];

    if(rank==0) memset(rigaSopra,0,sizeof(int)*col);
    if(rank==size-1) memset(rigaSotto,0,sizeof(int)*col);

    // if (rank==0){
    //     for(size_t i = 0;i<size;i++){
    //         printf("Disposizione[%ld] = %d\n",i,disposizione[i]); 
    //         printf("numElementi[%ld] = %d\n",i,num_elementi[i]);
    //     }
    // }

    Control(MPI_Scatterv(mat,num_elementi,disposizione,MPI_INT,ricezione,num_elementi[rank],MPI_INT,0,MPI_COMM_WORLD));
    
    MPI_Request handler_invio_sopra = MPI_REQUEST_NULL;
    if(rank!=0){
        Control(MPI_Isend(
            ricezione,
            col,
            MPI_INT,
            rank-1,
            0,
            MPI_COMM_WORLD,
            &handler_invio_sopra));
        log_debug("il rank %d sopra iSend",rank);
    }

    MPI_Request handler_invio_sotto = MPI_REQUEST_NULL;
    if(rank!=size-1){
        Control(MPI_Isend(
            ultima_riga(ricezione,num_elementi[rank],col),
            col,
            MPI_INT,
            rank+1,
            1,
            MPI_COMM_WORLD,
            &handler_invio_sotto));
        log_debug("il rank %d sotto iSend",rank);

    }

    int *array_ris = malloc(num_elementi[rank]*sizeof(int));
    memset(array_ris,0,num_elementi[rank]*sizeof(int));
    
    for(size_t s = 0;s<S;s++){

        if(rank!=size-1){
                    Control(MPI_Recv(
                                rigaSotto,
                                col,
                                MPI_INT,
                                rank+1,
                                0,
                                MPI_COMM_WORLD,
                                MPI_STATUS_IGNORE));
                    log_debug("il rank %d sotto Recv",rank);
        }

        if(rank!=0) {
            Control(MPI_Recv(
                        rigaSopra,
                        col,
                        MPI_INT,
                        rank-1,
                        1,
                        MPI_COMM_WORLD,
                        MPI_STATUS_IGNORE));
            log_debug("il rank %d sopra Recv",rank);
        }
            

        


        for(size_t i = 0 ; i < num_elementi[rank] ; i++){
            if(0<=i<col){
                array_ris[i]+=rigaSopra[i];
            }else{
                array_ris[i]+=ricezione[i-col];
            }
            
            if(num_elementi[rank]-col<=i<num_elementi[rank]){
                array_ris[i]+=rigaSotto[i%col];
            }else{
                array_ris[i]+=ricezione[i+col];
            }

            if(i%col!=0) array_ris[i]+=ricezione[i-1];

            if(i%col!=col-1) array_ris[i]+=ricezione[i+1];    
        }

        int *appoggio = ricezione;
        ricezione = array_ris;
        array_ris = appoggio;
        memset(array_ris,0,num_elementi[rank]*sizeof(int));

        if(s<S-1){
            Control(MPI_Wait(&handler_invio_sopra,MPI_STATUS_IGNORE));
            if(rank!=0){
                Control(MPI_Isend(
                    ricezione,
                    col,
                    MPI_INT,
                    rank-1,
                    0,
                    MPI_COMM_WORLD,
                    &handler_invio_sopra));
            }

            Control(MPI_Wait(&handler_invio_sotto,MPI_STATUS_IGNORE));
            if(rank!=size-1){
                Control(MPI_Isend(
                    ultima_riga(ricezione,num_elementi[rank],col),
                    col,
                    MPI_INT,
                    rank+1,
                    1,
                    MPI_COMM_WORLD,
                    &handler_invio_sotto));
            }
        }
        
    }

    Control(MPI_Gatherv(
        ricezione,
        num_elementi[rank],
        MPI_INT,
        mat,
        num_elementi,
        disposizione,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    ));

    //if(rank==0) printMatrix(mat,rig,col);

    Control(MPI_Finalize());
    return 0;
}
