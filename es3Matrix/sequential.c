#include <stdio.h>


#include "utils/macros.h"
#include "utils/random.h"
#include "utils/matrix.h"
#include "utils/logging.h"

typedef struct {
    int i;
    int j;
} Point;

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

int main(int argc, char **argv) {

    size_t col = 0;
    size_t rig = 0;
    size_t S = 0;


    if (argc==4){
        col = atol(argv[1]);
        rig = atol(argv[2]);
        S = atol(argv[3]);
        fatal_if((col == 0 || rig == 0 || S == 0),"un argomento non e un numero o e zero");
    }else{
        log_fatal("numero di argomenti sbagliato");
    }



    //init_random();
    srand(2476063558);
    int *mat1 = generate_random_matrix(rig,col,-5,5);
    int *mat2 = (int*)malloc((rig*col)*sizeof(int));
    //printMatrix(mat1,rig,col);
        for (size_t s = 0 ; s<S ; s++){
            for (size_t i = 0;i<rig;i++){
                for(size_t j =0;j<col;j++){
                    Point p = {
                        .i = i,
                        .j = j
                    };
                    size_t n = pointToNum(p,col);
                    //printf("n: %lu\n",n);
                    int sum = 0;
                    if (i>0){
                        Point x = {
                            .i = i - 1,
                            .j = j
                        };
                        size_t nn = pointToNum(x,col);
                        //printf("i > 0 nn: %lu\n", nn);
                        sum += mat1[nn];
                    }
                    if (i<rig-1){
                        Point x = {
                            .i = i + 1,
                            .j = j
                        };
                        size_t nn = pointToNum(x,col);
                        //printf("i < rig-1 nn: %lu\n", nn);
                        sum += mat1[nn];
                    }
                    if (j>0){
                        Point x = {
                            .i = i,
                            .j = j - 1
                        };
                        size_t nn = pointToNum(x,col);
                        //printf("j > 0 nn: %lu\n", nn);
                        sum += mat1[nn];
                    }
                    if (j<col-1){
                        Point x = {
                            .i = i,
                            .j = j + 1
                        };
                        size_t nn = pointToNum(x,col);
                        //printf("j < col - 1 nn: %lu\n", nn);
                        sum += mat1[nn];
                    }
                    mat2[n] = sum;
                }
            }
            printMatrix(mat1,rig,col);
            printf("\n");
            int *appoggio = mat1;
            mat1 = mat2;
            mat2 = appoggio;
        }
        printMatrix(mat1,rig,col);
    
    return 0;
}