#include <stdio.h>
#include <stdlib.h>

#include <alloca.h>
#include <time.h>

#include <mpi.h>

#define LOGGING_IMPLEMENTATION
#define STRINGS_IMPLEMENTATION
#include "utils/macros.h"
#include "utils/logging.h"
#include "utils/strings.h"

#define LEN 40

typedef struct {
    int *data;
    size_t lenght;
    size_t capacity;
} DA_Int;

void print_vec(int *vec, size_t len) {
    for(size_t i = 0; i<len; i++) {
        eprintf("%d",vec[i]);
        if(i != LEN - 1)
            eprintf(", ");        
    }
    eprintf("\n");
    fflush(stderr);
}

int *generate_vector(size_t len) {

    DA_Int vec = {0};

    vec.data = malloc(len*sizeof(int));
    fatal_if(vec.data == NULL,"Out of memory, buy more RAM");
    vec.capacity = len;
    for(size_t i = 0; i < len; i++) {
        append(&vec, (rand() % 10));
    }

    print_vec(vec.data, vec.lenght);

    return vec.data;

}


int main(int argc, char **argv) {
    Control(MPI_Init(&argc, &argv));

    int rank, size;
#ifdef DEBUG
    set_log_level(LOG_DEBUG);
    //setvbuf(stdout, NULL, _IONBF, 0);
    //setvbuf(stderr, NULL, _IONBF, 0);
#endif // DEBUG

    Control(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    Control(MPI_Comm_size(MPI_COMM_WORLD, &size));

    fatal_if(LEN % size != 0, "The Len must divide the process");
    size_t length = LEN/size; //TODO: la lunghezza potrebbe non essere divisibile per il numero di processi
    int *array1 = NULL;
    int *array2 = NULL;
    if(rank == 0) {
        unsigned int seed = time(NULL);
        printf("Seed: %u\n", seed);
        srand(seed);
        array1 = generate_vector(LEN);
        array2 = generate_vector(LEN);
        fatal_if(LEN % size != 0, "the number of process must devide the array size");
    }
    Control(MPI_Barrier(MPI_COMM_WORLD));
    double start = MPI_Wtime();

    if(rank != 0) {
        array1 = malloc(length*sizeof(*array1));
        fatal_if(array1 == NULL, "Out of memory, buy more RAM");
        array2 = malloc(length*sizeof(*array2));
        fatal_if(array2 == NULL, "Out of memory, buy more RAM");
    }

    Control(MPI_Scatter(array1, length, MPI_INT, array1, length, MPI_INT, 0, MPI_COMM_WORLD));
    Control(MPI_Scatter(array2, length, MPI_INT, array2, length, MPI_INT, 0, MPI_COMM_WORLD));

#ifdef DEBUG
    eprintf("Rank %d ", rank);
    print_vec(array1, length);
    eprintf("Rank %d ", rank);
    print_vec(array2, length);
    fflush(stderr);
#endif

    // procediamo con il calcolo
    int result[length];
    for(size_t i=0; i<length; i++) {
        result[i] = array1[i] + array2[i];
    }

#ifdef DEBUG
    eprintf("Rank %d, risultato: ", rank);
    print_vec(result, length);
    fflush(stderr);
#endif

    Control(MPI_Gather(result, length, MPI_INT, array1, length, MPI_INT, 0, MPI_COMM_WORLD));

    Control(MPI_Barrier(MPI_COMM_WORLD));
    double end = MPI_Wtime() - start;
    double time_result = 0;
    Control(MPI_Reduce(
        &end, &time_result, 1,
        MPI_DOUBLE, MPI_MAX,
        0, MPI_COMM_WORLD
    ));

    if(rank == 0) {
        eprintf("risultato finale: ");
        for(size_t i = 0; i < LEN; i++) {
            eprintf("%d",array1[i]);
            if(i != LEN - 1)
                putc(',',stderr);
        }
        putc('\n',stderr);
        eprintf("Tempo calcolo: %lf\n", time_result);
        fflush(stdout);
    }

    free(array1);
    free(array2);
    Control(MPI_Finalize());
    return 0;
}