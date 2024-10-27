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


typedef struct {
    Point *data;
    size_t length;
    size_t capacity;
}DA_points;

DA_points findPointsModulo(int rank,int size,size_t rig,size_t col){
    DA_points out = {0};
    size_t tot_point = rig*col;
    for(size_t iter = rank ; iter<tot_point;iter+=size){
        size_t x = iter/col;
        size_t y = iter%col;
        if (x<rig && y<col){
            Point pt = {
                .i = x,
                .j = y
            };
            append(&out,pt);
        }
    }
    return out;
}

void findPointsContiguo(int rank,int size,size_t rig,size_t col,size_t *inizio,size_t *fine){
    size_t tot_point = rig*col;
    size_t points_for_rank = tot_point/size;
    size_t remaning_points = tot_point%size;

    if ((size_t)rank<remaning_points){
        *inizio = (rank * (points_for_rank+1));
        *fine = *inizio + points_for_rank;
    }else{
        *inizio = (remaning_points * (points_for_rank+1))+
                ((rank-remaning_points)*points_for_rank);
        *fine = *inizio + points_for_rank-1;
    }  
    
}

void arrayDisposizione(int size,size_t rig,size_t col,size_t arrOut[size]){
    size_t tot_point = rig*col;
    size_t points_for_rank = tot_point/size;
    size_t remaning_points = tot_point%size;

    for(size_t rank = 0;rank<size;rank++){
        if ((size_t)rank<remaning_points){
            arrOut[rank] = (rank * (points_for_rank+1));
            
        }else{
            arrOut[rank] = (remaning_points * (points_for_rank+1))+
                    ((rank-remaning_points)*points_for_rank);
            
        }  
    }
    
}void arrayNumElementi(int size,size_t rig,size_t col,size_t arrOut[size]){
    size_t tot_point = rig*col;
    size_t points_for_rank = tot_point/size;
    size_t remaning_points = tot_point%size;
    size_t inizio;
    size_t fine;

    for(size_t rank = 0;rank<size;rank++){
        if ((size_t)rank<remaning_points){
            inizio = (rank * (points_for_rank+1));
            fine = inizio + points_for_rank;
        }else{
            inizio = (remaning_points * (points_for_rank+1))+((rank-remaning_points)*points_for_rank);
            fine = inizio + points_for_rank-1;
        }
        arrOut[rank]=fine-inizio;  
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

void stampaPunti(DA_points *punti,int rk){
    foreach(Point,p,punti){
        printf("rank %d (x: %d ,y: %d)\n",rk,p->i,p->j);

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
        init_random();
        mat = generate_random_matrix(rig,col,-5,5);


    }else{
        mat = malloc((rig*col)*sizeof(int));
        fatal_if(mat==NULL,MSG_ERR_FULL_MEMORY);
    };

    Control(MPI_Bcast(mat,rig*col,MPI_INT,0,MPI_COMM_WORLD));

    size_t disposizione[size];
    size_t num_elementi[size];

    arrayNumElementi(size,rig,col,num_elementi);
    arrayDisposizione(size,rig,col,disposizione);



    size_t inizio = disposizione[rank];
    size_t fine = disposizione[rank]+num_elementi[rank];
    size_t lunghezza = num_elementi[rank]*sizeof(int);
    int *ranksolution = malloc(lunghezza);
    for (size_t s = 0 ; s<S ; s++){
        for(size_t i = inizio ; i<=fine ; i++){
            Point p = numToPoint(i,col);
            int sum = 0;
            if (p.i>0){sum += mat[((p.i-1)*rig)+p.j];}
            if (p.i<rig-1){sum += mat[((p.i+1)*rig)+p.j];}
            if (p.j>0){sum += mat[((p.i)*rig)+p.j-1];}
            if (p.j<col-1){sum += mat[((p.i)*rig)+p.j+1];}
            ranksolution[i-inizio] = sum;
        }
        MPI_Allgatherv(ranksolution,lunghezza,MPI_INT,mat,num_elementi,disposizione,MPI_INT,MPI_COMM_WORLD);
    }
    


    Control(MPI_Finalize());
    return 0;
}