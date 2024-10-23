#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <string.h>

#define LOGGING_IMPLEMENTATION
#include "utils/logging.h"

#include "matrix.h"

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
    unsigned int seed = 1729566597;
    srand(seed);
    log_info("Seed = %u", seed);

    //TODO: permettere all'utente di definirlo
    bool print_output = true;

    size_t order = parse_order_matrix(argc, argv);
    int *mtx1 = generate_matrix(order);
    int *mtx2 = generate_matrix(order);

    if(log_level <= LOG_DEBUG) {
        eprintf("Prima matrice:\n");
        eprintMatrix(mtx1, order);
        eprintf("Seconda matrice:\n");
        eprintMatrix(mtx2, order);
    }

    int *result = calculate_matrix_mul(mtx1, mtx2, order);

    if(log_level <= LOG_DEBUG) {
        eprintf("Matrice risultato:\n");
        eprintMatrix(result, order);
    }

    clock_t elapsed = clock() - start;

    double time_taken = ((double)elapsed)/CLOCKS_PER_SEC;

    free(mtx2);
    free(mtx1);

    eprintf("Tempo messo: %lf\n", time_taken);
    if(print_output)
        printMatrix(result, order);
    free(result);
    return 0;
}