#define _GNU_SOURCE // IMPORTANTE: lasciare queste tre righe 
#include <pthread.h>
#include <sched.h>

#include <stdio.h>
#include <stdlib.h>

#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


#include "utils/logging.h"

// ATTENZIONE!!! è roba constante
typedef struct {
    long thread_size;
    long dcache_line_size;
} Global_Contex;

// NON MODIFICARE ASSOLUTAMENTE QUESTA VARIABILE, LEGGI SOLTANTO (TODO: non so come controllare questa cosa a tempo di compilazione)
static Global_Contex glob_ctx = {0};

void *run(void *arg) {
    int *rank = arg;
    printf("Hello World from: %d, size: %ld\n",*rank, glob_ctx.thread_size);
    return NULL;
}

void parse_args(int argc, char **argv) {
    glob_ctx.dcache_line_size = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
    fatal_if(glob_ctx.dcache_line_size == -1, "syscall error: %s", strerror(errno));
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

    pthread_t *threads = malloc(glob_ctx.thread_size*sizeof(pthread_t));
    int *args = malloc(glob_ctx.thread_size*sizeof(*args)); //TODO: passare gli argomenti usando posix_memalign con i giusti argomenti per evitare che gli argomenti siano sulla stessa linea della cache

    for(int i=0; i < glob_ctx.thread_size; i++) {
        args[i] = i;
        pthread_create(&threads[i],NULL,run,&args[i]);
    }

    for(int i=0; i < glob_ctx.thread_size; i++) {
        pthread_join(threads[i],NULL);
    }

    free(threads);
    free(args);
    return 0;
}