#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <string.h>

#define LOGGING_IMPLEMENTATION
#include "utils/logging.h"

void printMatrix(int *mtx, size_t order) {
    for(size_t i=0; i < order; i++) {
        eprintf("    ");
        for(size_t j=0; j < order; j++) {
            eprintf("%d", mtx[i*order + j]);
            if(j == order - 1)
                eputc('\n');
            else eprintf(" ,");
        }
    }
}

size_t parse_order(int argc, char **argv) {
    char *program_name = argv[0];
    (void) program_name;
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
        result[i] = (rand() % 6) - 1;
    }
    return result;
}

int dot_product(int *vec1, int *vec2,  size_t len) {
    int result = 0;
    for(size_t i = 0; i < len; i++) {
        result += vec1[i]*vec2[i];
    }
    return result;
}

void reverse_matrix(int *mtx, size_t order) {
    int app = 0;
    for(size_t i = 0; i < order; i++) {
        for(size_t j = 0; j < order; j++) {
            if(i < j) {
                size_t index = i*order + j;
                app = mtx[index];
                mtx[index] = mtx[j*order + i];
                mtx[j*order + i] = app;
            }
        }
    }
}

int *calculate_matrix_mul(int *m1, int *m2, size_t order) {
    size_t total_len = order*order;
    size_t n_bytes = total_len*sizeof(int);
    int *result = malloc(n_bytes);
    memset(result, -1, n_bytes);

    reverse_matrix(m2, order);
    
    for(size_t i=0; i<order; i++) {
        for(size_t j=0; j<order; j++) {
            int *row1 = m1+(order*i);
            int *row2 = m2+(order*j);
            int dotprod = dot_product(row1,row2,order);
            result[order*i + j] = dotprod;
        }
    }
    
    return result;
}

int main(int argc, char **argv) {

    clock_t start = clock();

#ifdef DEBUG
    set_log_level(LOG_DEBUG);
#endif // DEBUG
    unsigned int seed = time(NULL);
    srand(1729566597);
    log_info("Seed = %u", seed);
    size_t order = parse_order(argc, argv);
    int *mtx1 = generate_matrix(order);
    int *mtx2 = generate_matrix(order);

    if(log_level <= LOG_DEBUG) {
        eprintf("Prima matrice:\n");
        printMatrix(mtx1, order);
        eprintf("Seconda matrice:\n");
        printMatrix(mtx2, order);
    }

    int *result = calculate_matrix_mul(mtx1, mtx2, order);

    if(log_level <= LOG_DEBUG) {
        eprintf("Matrice risultato:\n");
        printMatrix(result, order);
    }

    clock_t elapsed = clock() - start;

    double time_taken = ((double)elapsed)/CLOCKS_PER_SEC;

    free(result);
    free(mtx2);
    free(mtx1);

    eprintf("Tempo messo: %lf\n", time_taken);
    
    return 0;
}