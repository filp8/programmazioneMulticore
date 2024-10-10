#include <stdio.h>

#include <mpi.h>

#include "utils/macros.h"
#define LOGGING_IMPLEMENTATION
#include "utils/logging.h"

typedef double(*Function)(double);

double identity(double x) {
    return x;
}

double quadratic(double x) {
    return x*x;
}

#include <math.h>

double exponetial(double x) {
    return exp(x);
}

double calculate_local_trapezoidal_result(
    double local_a, double local_b, int local_n,
    int rank, Function f
) {
    fatal_if(local_b < local_a,
        "local_b is less than local_a in rank %d", rank);

    double h = (local_b - local_a)/local_n;
    double result = (f(local_a) + f(local_b))/2;
    double x = local_a;

    for(int i = 0; i < local_n; i++) {
        double to_add = f(x);
        if(!(i != 0 && i != local_n)) {
            result += to_add/2;
        } else result += to_add;
        x += h;
    }

    return h*result;
}

typedef struct {
    double a;
    double b;
    double h;
    long n;
} Contex;

Contex parse_args(int argc, char **argv) {
    //TODO: permettere all'utente di inserire l'input n.

    char *program = argv[0];

    fatal_if(argc != 4
        ,"Gli argomenti di %s devono essere 3, sono stati inseriti: %d", program,argc - 1);

    Contex ctx = {
        .a = atof(argv[1]),
        .b = atof(argv[2]),
        .n = atoi(argv[3]),
    };
    ctx.h = (ctx.b-ctx.a)/ctx.n;

    return ctx;
}

#define A__ (double)0
#define B__ (double)1
#define N__ 1024

int main(int argc, char **argv) {

    //Contex ctx = parse_args(argc, argv);
    int rank, size;

    Control(MPI_Init(&argc, &argv));

    set_log_level(LOG_DEBUG);

    Control(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    Control(MPI_Comm_size(MPI_COMM_WORLD, &size));
    
    double h = (B__- A__)/N__;

    int local_n = N__/size;
    double slice = h*local_n;
    double local_a = A__ + rank*slice;
    double local_b = local_a + slice;

    log_debug("rank %d A:%lf B:%lf N:%d",rank,local_a,local_b,local_n);

    double local_result = calculate_local_trapezoidal_result(local_a, local_b, local_n ,rank ,exponetial);
    log_debug("rank %d local result: %lf",rank,local_result);
    double total_result;

    Control(MPI_Reduce(
        &local_result, &total_result,
        1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD
    ));

    if(rank == 0) {
        printf("Il risultato totale è: %lf\n", total_result);
    }

    Control(MPI_Finalize());
    return 0;
}