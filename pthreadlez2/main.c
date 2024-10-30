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
#include <string.h>

#include "utils/logging.h"

typedef struct {
    long thread_size;
    int n;
} Global_Contex;

static Global_Contex glob_ctx = {0};

typedef struct {
    int rank;
} Thread_Contex;

typedef struct {
    double partial_sum;
} Thread_Output;

void *run(void *arg) {
    Thread_Contex *ctx = (Thread_Contex*)arg;
    Thread_Output *out = malloc(sizeof(*out));
    memset(out, 0, sizeof(*out));

    int local_m = glob_ctx.n/glob_ctx.thread_size;
    int start = local_m*ctx->rank;
    int end = local_m*(1+ctx->rank);
    double segno = glob_ctx.n % 2 == 0 ? 1 : -1;
    for(int i = start; i < end; i++) {
        out->partial_sum+= segno*(double)1/(2*(double)i + 1);
        segno = (-1)*segno;
    }
    return out;
}

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

int main(int argc, char **argv) {
    parse_args(argc, argv);

    glob_ctx.n = glob_ctx.thread_size*10000000000000;
    
    fatal_if(!(glob_ctx.n % glob_ctx.thread_size == 0 && glob_ctx.n >= glob_ctx.thread_size),
        "Non hai messo il numero giusto di n rispetto ai thread,n: %d,size: %d",
        glob_ctx.n ,glob_ctx.thread_size);
    pthread_t *threads = malloc(glob_ctx.thread_size*sizeof(pthread_t));


    Thread_Contex *args = malloc(glob_ctx.thread_size*sizeof(*args));

    for(int i=0; i < glob_ctx.thread_size; i++) {
        args[i].rank = i;
        pthread_create(&threads[i],NULL,run,&args[i]);
    }

    double total_sum = 0;
    for(int i=0; i < glob_ctx.thread_size; i++) {
        Thread_Output *out;
        pthread_join(threads[i],(void**)&out);
        total_sum += 4*out->partial_sum;
        free(out);
    }

    printf("PI Greco: %lf\n", total_sum);


    free(threads);
    free(args);
    return 0;
}