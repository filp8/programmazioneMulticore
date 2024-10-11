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

    char *endptr = NULL;
    char *program = argv[0];

    fatal_if(argc != 4,
        "Gli argomenti di %s devono essere 3, sono stati inseriti: %d", program,argc - 1);

    double a = strtod(argv[1], &endptr);
    fatal_if(*endptr != '\0',
        "error occured while parsing the 'a' argument: %s", strerror(errno));

    double b = strtod(argv[2], &endptr);
    fatal_if(*endptr != '\0',
        "error occured while parsing the 'b' argument: %s", strerror(errno));

    long n = strtol(argv[3], &endptr, 10);
    fatal_if(*endptr != '\0',
        "error occured while parsing the 'n' argument: %s", strerror(errno));

    Contex ctx = {
        .a = a,
        .b = b,
        .n = n,
    };
    ctx.h = (ctx.b-ctx.a)/(double)ctx.n;

    return ctx;
}

int main(int argc, char **argv) {

#ifdef DEBUG
    set_log_level(LOG_DEBUG);
#endif

    Contex ctx = parse_args(argc, argv);
    int rank, size;

    Control(MPI_Init(&argc, &argv));

    Control(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    Control(MPI_Comm_size(MPI_COMM_WORLD, &size));

    int local_n = ctx.n/size;
    double slice = ctx.h*local_n;
    double local_a = ctx.a + rank*slice;
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