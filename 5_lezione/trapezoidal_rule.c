// Area del trapeziode: h/2*[f(x_i) + f(x_{i+1})]
// h = (b - a)/n NOTA: h non è l'altezza, ma la dimensione della base
// x_0 = a x_1 = a + h, x_2 = a + 2h
// facciamo la somma delle aree di questo trapezio
// ma la somma sarà: h[f(x_0)/2 + f(x_1) + ... + f(x_n)/2]
#include <stdlib.h>
#include <stdio.h>

#include <alloca.h>

#include <mpi.h>

#include "utils/macros.h"
#define LOGGING_IMPLEMENTATION
#include "utils/logging.h"

double identity(double x) {
    return x;
}

double quadratic(double x) {
    return x*x;
}

typedef double(*Function)(double);

double calculate_local_trapezoidal_result(
    double a, double h,
    int rank, int size,
    Function f
) {
    double result;
    if(rank == 0) {
        result = f(a) + f(a + h);
    } else{
        double x = a + (rank+1)*h;
        if(rank == size - 1) {
            result = f(x)/2;
        } else result = f(x);
    }
    return result;
}

typedef struct {
    double a;
    double b;
    long n;
} Contex;

Contex parse_args(int argc, char **argv) {

    if(argc != 4) {
        log_error("Gli argomenti devono essere 3, sono stati inseriti: %d",argc - 1);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    char *program = argv[0];

    Contex ctx;
    ctx.a = strtol(argv[1]);
    ctx.b = atof(argv[2]);
    ctx.n = atoi(argv[3]);

    return ctx;
}

int main(int argc, char **argv) {

    //TODO: permettere all'utente di inserire l'input n.
    Contex ctx = parse_args(argc, argv);
    int rank, size;

    Control(MPI_Init(&argc, &argv));

    set_log_level(LOG_DEBUG);

    Control(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    Control(MPI_Comm_size(MPI_COMM_WORLD, &size));

    double ab[2] = {0}; 

    if(rank == 0) {
        printf("Inserire a: ");
        fflush(stdout);
        scanf("%lf",&ab[0]);
        printf("Inserire b: ");
        fflush(stdout);
        scanf("%lf",&ab[1]);
        if(ab[0] < ab[1])
            printf("partendo da %lf arrivando a %lf\n",ab[0],ab[1]);
        else {
            log_error("a deve essere < b");
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
        //TODO: utilizzare MPI_Bcast
        for(int i=1; i < size; i++)
            Control(MPI_Send(ab, 2, MPI_DOUBLE, i, 0, MPI_COMM_WORLD));
    } else {
        Control(
            MPI_Recv(ab, 2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD ,MPI_STATUS_IGNORE)
        );
    }

    double h = (ab[1] - ab[0])/size;

    double local_result = calculate_local_trapezoidal_result(ab[0], h, rank, size, quadratic);
    double total_result;

    Control(MPI_Reduce(
        &local_result, &total_result,
        1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD
    ));

    if(rank == 0) {
        double final_result = h*total_result;
        printf("Il risultato totale è: %lf\n", final_result);
    }

    Control(MPI_Finalize());
    return 0;
}