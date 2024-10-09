#include <stdio.h>
#include <mpi.h>

// Esercizio: approssimare l'integrale su una funzione

// Area del trapeziode: h/2*[f(x_i) + f(x_{i+1})]

// h = (b - a)/n NOTA: h non è l'altezza, ma la dimensione della base

// x_0 = a x_1 = a + h, x_1 = a + 2h

// facciamo la somma delle aree di questo trapezio

// ma la somma sarà: h[f(x_0)/2 + f(x_1) + ... + f(x_n)/2]

// l'input permette solo al rank zero di leggere lo standard input
// necessariamente deve essere fatto MPI_INIT prima di leggere l'input

#include "utils/macros.h"

int main(void) {

    
    int size, rank;

    int a,b;
    
    Control(MPI_Init(NULL, NULL));
    Control(MPI_Comm_size(MPI_COMM_WORLD, &size));
    Control(MPI_Comm_Size(MPI_COMM_WORLD, &rank));

    if(rank == 0) {
        printf("Inserisci a:");
        scanf("%f",a);
    }

    // chiamate collettive, fin ora abbiamo visto solo comunciazioni punto punto

    /*int MPI_Reduce(const void *sendbuf, void *recvbuf, int count,
               MPI_Datatype datatype, MPI_Op op, int root,
               MPI_Comm comm)*/

    // si possono creare anche operazioni

    return 0;
}