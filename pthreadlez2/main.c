// i*m/t fino a (i+1)*m/t - 1
// i è l'iesimo thread
// m è il numero totale di thread
// t è il numero di colonn
// questo è il modo per dividere le colonne tra i thread
// 
// Ogni thread esegue una funzione passata per parametro.
// 
// pigreo = 4*(1-1/3+1/5-...\sum_{n=0}^n (-1)^n*1/(1/2*n + 1))

#include <stdio.h>
#include <stdlib.h>

#include <getopt.h>

#include <pthread.h>
#include <semaphore.h>

#include <string.h>

#include "utils/logging.h"

typedef struct {
    long thread_size;
    size_t n;
    pthread_mutex_t sum_mutex;
    double total_sum;
} Global_Contex;

static Global_Contex glob_ctx = {0};

typedef struct {
    int rank;
} Thread_Contex;

#define MAX_N (size_t)5*1024*1024*1024

typedef struct {
    // double partial_sum;
} Thread_Output;

void parse_args(int argc, char **argv) {
    int opt = 0;
    char *program_name = argv[0];
    char *number;
    bool assignedThreadcount = false;
    while((opt = getopt(argc, argv, "t:")) != -1) {
        switch (opt)
        {
        case 't':
            assignedThreadcount = true;
            number = optarg;
            fatal_if(number == 0, "opzione non letta");
            char *endptr;
            long size = strtol(number, &endptr, 10);
            fatal_if(*endptr != '\0',
                "l'argomento è un numero non valido: %s", number);
            glob_ctx.thread_size = size;
            break;
        
        case '?':
            log_error("Opzione non esistente: %c", opt);
            log_info("%s -t <thread number>",program_name);
            exit(2);
            break;
        default:
            log_fatal_err(1, "Opzione non riconosciuta da getopt: %c", opt);
        }
    }
    if(!assignedThreadcount) {
        log_error("-t obbligatorio");
        log_info("%s -t <thread number>",program_name);
    }
        
}

void *run(void *arg) {
    // TODO: aggiungere il fatto che n non divide la grandezza dei thread
    Thread_Contex *ctx = (Thread_Contex*)arg;
    double partial_sum = 0;

    size_t local_m = glob_ctx.n/glob_ctx.thread_size;
    size_t start = local_m*ctx->rank;
    size_t end = local_m*(1+ctx->rank);
    double segno = glob_ctx.n % 2 == 0 ? 1 : -1;
    for(size_t i = start; i < end; i++) {
        partial_sum+= segno*(double)1/(2*(double)i + 1);
        segno = (-1)*segno;
    }
    pthread_mutex_lock(&glob_ctx.sum_mutex);
    glob_ctx.total_sum += 4*partial_sum;
    pthread_mutex_unlock(&glob_ctx.sum_mutex);

    return NULL;
}

static inline void init_mutexs(void) {
    pthread_mutex_init(&glob_ctx.sum_mutex, NULL);
}

int main(int argc, char **argv) {
    parse_args(argc, argv);
    init_mutexs();
    glob_ctx.n = MAX_N;
    
    fatal_if(!(glob_ctx.n % glob_ctx.thread_size == 0 && glob_ctx.n >= (size_t)glob_ctx.thread_size),
        "Non hai messo il numero giusto di n rispetto ai thread,n: %d,size: %d",
        glob_ctx.n ,glob_ctx.thread_size);
    pthread_t *threads = malloc(glob_ctx.thread_size*sizeof(pthread_t));


    Thread_Contex *args = malloc(glob_ctx.thread_size*sizeof(*args));

    for(int i=0; i < glob_ctx.thread_size; i++) {
        args[i].rank = i;
        pthread_create(&threads[i],NULL,run,&args[i]);
    }

    // TODO: permettere ai thread di sommare una variabile globale per togliere questo for
    for(int i=0; i < glob_ctx.thread_size; i++) {
        pthread_join(threads[i],NULL);
    }

    printf("PI Greco: %lf\n", glob_ctx.total_sum);


    free(threads);
    free(args);
    return 0;
}