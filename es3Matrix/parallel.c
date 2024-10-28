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
    //memset(sendcounts,0,comm_size*sizeof(*sendcounts));
    int row_for_rank  = rows/comm_size;
    //printf("rows_for_rank: %d\n",rows);
    int row_remaining = rows%comm_size;
    for(int rank=0; rank<comm_size; rank++) {
        int to_sum = 0;
        if(rank < row_remaining) to_sum++;
        to_sum += row_for_rank*cols;
        sendcounts[rank] = to_sum;
    }
}

void arrayDisposizioneInvio(
    int comm_size, int* displs,int *sendcounts
) {
    //memset(displs,0,comm_size*sizeof(*displs));

    for(int rank=1; rank<comm_size; rank++) {
        displs[rank] = sendcounts[rank-1] + displs[rank-1];
    }
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
    } else {
        mat = malloc(rig*col*sizeof(*mat));
        fatal_if(mat == NULL, MSG_ERR_FULL_MEMORY);
    }

    Control(MPI_Bcast(mat,rig*col,MPI_INT,0,MPI_COMM_WORLD));

    //Control(MPI_Bcast(mat,rig*col,MPI_INT,0,MPI_COMM_WORLD));
    int num_elementi[size];
    int disposizione[size];

    /*arrayNumElementiInvio(size,sendcounts,rig,col);
    arrayDisposizioneInvio(size,displs,sendcounts);
    if(rank == 0) {
        for(int r=0;r<size;r++) {
            printf("sendcounts[%d] = %d\n",r,sendcounts[r]);
            printf("displs[%d] = %d\n",r,displs[r]);
        }
    }*/

    arrayNumElementiRicezione(size,rig,col,num_elementi);
    arrayDisposizioneRicezione(size,rig,col,disposizione);

    //MPI_Scatterv(mat, NULL, NULL, MPI_INT, NULL, 0,MPI_INT,0,MPI_COMM_WORLD);
    // *********************
    // +++++++++++++++++++++
    // *********************
    // +++++++++++++++++++++

    size_t inizio = disposizione[rank];
    size_t fine = disposizione[rank]+num_elementi[rank];
    int *ranksolution = malloc(num_elementi[rank]*sizeof(int));
    for (size_t s = 0 ; s<S ; s++){
        if(rank==0){printMatrix(mat,rig,col);printf("\n");}
        for(size_t i = inizio ; i<=fine ; i++){
            
            Point p = numToPoint(i,col);
            int sum = 0;
            if (p.i>0){sum += mat[((p.i-1)*col)+p.j];}
            if ((size_t) p.i<rig-1){sum += mat[((p.i+1)*col)+p.j];}
            if (p.j>0){sum += mat[((p.i)*col)+p.j-1];}
            if ((size_t)p.j<col-1){sum += mat[((p.i)*col)+p.j+1];}
            ranksolution[i-inizio] = sum;
        }
        Control(MPI_Allgatherv(ranksolution,num_elementi[rank],
                MPI_INT,mat,
                num_elementi,disposizione,MPI_INT,MPI_COMM_WORLD));
                //S*log(size)
                //S*2
        }

        
    if(rank==0){printMatrix(mat,rig,col);}
    Control(MPI_Finalize());
    return 0;
}

#if 0
typedef struct {
    Point *data;
    size_t length;
    size_t capacity;
}DA_points;



#endif // ignorato