#include <stdio.h>

#include <alloca.h>

#include <mpi.h>

#include "utils/macros.h"
#define LOGGING_IMPLEMENTATION
#include "utils/logging.h"
#define STRINGS_IMPLEMENTATION
#include "utils/strings.h"

typedef struct {
    double *data;
    size_t lenght;
    size_t capacity;
} DA_Double;

DA_Double get_vector_from_input(void) {

    DA_Double result = {0};

    printf("Insert vector\nUse ',' as delimit for numbers and for stopping input write ';'\n: ");
    fflush(stdout);

    String_Builder input = {0};
    char c;
    //TODO: usare la syscall read che è meglio
    while((c = getchar()) != ';') {
        append(&input, c);
    }
    String_View sv_input = sv_from_sb(&input);
    String_Builder cstr_number = {0};
    char *endptr;

    while(sv_input.lenght > 0) {

        String_View sv_number = sv_chop_by_delim(&sv_input, ',');
        sb_append_sv(&cstr_number, sv_number);
        sb_to_cstr(&cstr_number);

        double number = strtod(cstr_number.data, &endptr);
        fatal_if(*endptr != '\0',
            "Number is invalid: %s", cstr_number.data);
        
        append(&result, number);
        cstr_number.lenght = 0;
    }

defer:
    free(input.data);
    if(cstr_number.data != NULL) free(cstr_number.data);
    return result;
}

int main(int argc, char **argv) {
    Control(MPI_Init(&argc, &argv));

    int rank, size;

    Control(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    Control(MPI_Comm_size(MPI_COMM_WORLD, &size));

    size_t total_len = 0;
    size_t length = 0;

    double *array1;
    double *array2;

    if(rank == 0) {
        DA_Double vec1 = get_vector_from_input();
        DA_Double vec2 = get_vector_from_input();
        fatal_if(vec1.lenght != vec2.lenght,
            "The input vectors must have the same size");
        total_len = vec1.lenght;
        length = vec1.lenght/size; //TODO: la lunghezza potrebbe non essere divisibile per il numero di processi
        array1 = vec1.data;
        array2 = vec2.data;
    }

    Control(MPI_Bcast(&length, 1, MPI_LONG, 0, MPI_COMM_WORLD));

    if(rank != 0) {
        array1 = malloc(length*sizeof(*array1));
        fatal_if(array1 == NULL, "Out of memory, buy more RAM");
        array2 = malloc(length*sizeof(*array2));
        fatal_if(array2 == NULL, "Out of memory, buy more RAM");
    }


    Control(MPI_Scatter(array1, length, MPI_DOUBLE, array1, length, MPI_DOUBLE, 0, MPI_COMM_WORLD));
    Control(MPI_Scatter(array2, length, MPI_DOUBLE, array2, length, MPI_DOUBLE, 0, MPI_COMM_WORLD));

    // procediamo con il calcolo
    double result[length];
    for(size_t i=0; i<length; i++) {
        result[i] = array1[i] + array2[i];
    }

    Control(MPI_Gather(result, length, MPI_DOUBLE, array1, total_len, MPI_DOUBLE, 0, MPI_COMM_WORLD));

    free(array1);
    free(array2);
    Control(MPI_Finalize());
    return 0;
}