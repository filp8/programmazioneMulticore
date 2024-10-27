#include <stdio.h>


#include "utils/macros.h"
#include "utils/random.h"
#include "utils/matrix.h"
#include "utils/logging.h"

int main(int argc, char **argv) {

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



    init_random();
    int *mat1 = generate_matrix_n_m(rig,col,-5,5);
    int *mat2 = (int*)malloc((rig*col)*sizeof(int));
        for (size_t s = 0 ; s<S ; s++){
            for (size_t i = 0;i<rig;i++){
                for(size_t j =0;j<col;j++){
                    int sum = 0;
                    if (i>0){
                    sum += mat1[((i-1)*rig)+j];
                    }
                    if (i<rig-1){
                    sum += mat1[((i+1)*rig)+j];
                    }
                    if (j>0){
                    sum += mat1[((i)*rig)+j-1];
                    }
                    if (j<col-1){
                    sum += mat1[((i)*rig)+j+1];
                    }
                    mat2[(i*rig)+j] = sum;
                }
            }
            printMatrix_n_m(mat1,rig,col);
            printf("\n");
            int *appoggio = mat1;
            mat1 = mat2;
            mat2 = appoggio;
        }
        printMatrix_n_m(mat1,rig,col);
    
    return 0;
}