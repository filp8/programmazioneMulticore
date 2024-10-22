#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <string.h>

#define LOGGING_IMPLEMENTATION
#include "utils/logging.h"

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
        result[i] = rand() % 10;
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

// NOTA IMPORTANTE!!!! :m2 deve essere trasposto
int *calculate_matrix_mul(int *m1, int *m2, size_t order) {
    size_t total_len = order*order;
    size_t n_bytes = total_len*sizeof(int);
    int *result = malloc(n_bytes);
    memset(result, -1, n_bytes);
    
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
    unsigned int seed = time(NULL);
    srand(seed);
    log_info("Seed = %u", seed);
    size_t order = parse_order(argc, argv);
    int *mtx1 = generate_matrix(order);
    int *mtx2 = generate_matrix(order);



    free(mtx2);
    free(mtx1);
    return 0;
}