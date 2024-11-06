#include <stdio.h>
#include <unistd.h>
#include <mpi.h>

#define LOGGING_IMPLEMENTATION
#define STRINGS_IMPLEMENTATION
#include "utils/macros.h"
#include "utils/logging.h"
#include "utils/strings.h"

int main(void) {
    /*int pid = fork();
    if(pid == 0) {
        printf("Processo figlio\n");
    } else {
        printf("Processo padre\n");
    }*/
    // single program multiple memory
    // il processo figlio avrà il codice uguale al padre, ma memoria diversa
    /*int r = MPI_Init(NULL, NULL);
    if(r != MPI_SUCCESS) {
        log_error("Error starting MPI program. Terminating\n");
        MPI_Abort(MPI_COMM_WORLD, r);
    }
    printf("Hello World!\n");
    MPI_Finalize(); */

    /*  ogni processo è un rank, se p sono i processi i rank vanno da 0 fino a p - 1
        In MPI usistyono dei comunicatori.
        In MPI esiste un comunicatore che è un insieme di processi.
        MPI di default crea un comunicatore comune per tutti i processi.
        Questo comunicatore si chiama MPI_COMM_WORLD.
        I comunicatori servono per isolare i processi per fare dei lavori comuni
        da questi processi.

        MPI_Comm_Size restituisce il numero di processi di un cominucatore
        MPI_Comm_rank restituisce il rank (l'identificatore) di un processo

    */

   /* int r = MPI_Init(NULL, NULL);
   int size, rank;
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if(r != MPI_SUCCESS) {
        log_error("Error starting MPI program. Terminating");
        MPI_Abort(MPI_COMM_WORLD, r);
    }
    printf("Sono il processo di size: %d! rank: %d\n",size, rank);
    MPI_Finalize();
    return 0; */

    // questa versione stampa i processi nell'ordine dei loro rank
    int r = MPI_Init(NULL, NULL);
    int size, rank;

    if(r != MPI_SUCCESS) {
        log_error("Error starting MPI program. Terminating");
        MPI_Abort(MPI_COMM_WORLD, r);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    char str[256];

    if(rank == 0) {
        printf("Sono il processo di size: %d! rank: %d\n",size, rank);
        for(int i = 1; i < size; i++) {
            MPI_Recv(str, 256,MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("%s",str);
        }
    } else {
        snprintf(str, 256,"Sono il processo di size: %d! rank: %d\n",size, rank);
        MPI_Send(str, 256, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    // per MPI_Send e MPI_Recv per poter metchare e fare con successo la trasmissione
    // allora il tag e il comunicatore deve essere uguale, anche la destinazione e la sorgente
    return 0; 
}