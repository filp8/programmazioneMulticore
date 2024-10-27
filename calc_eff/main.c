#include <stdio.h>

#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

#include "utils/macros.h"
#include "utils/logging.h"

typedef struct {
    char *program_name;
    char *parallel_program_path;
    char *serial_program_path;
} Contex;

static Contex ctx = {0};

static inline void usage(void) {
    log_info("Usage: %s -p <parallel program path> -s <serial program path>", ctx.program_name);
}

void parse_args(int argc, char **argv) {
    ctx.program_name = argv[0];
    int opt;
    while((opt = getopt(argc, argv, "p:s:")) != -1) {
        log_debug("opt: %d", opt);
        switch (opt)
        {
        case 'p':
            log_debug("Parsing p, optarg: %s", optarg);
            ctx.parallel_program_path = optarg;
            break;
        case 's':
            log_debug("Parsing s, optarg: %s", optarg);
            ctx.serial_program_path = optarg;
            break;
        case '?':
            log_error("Error while parsing arguments");
            usage();
            exit(1);
        }
    }
    if(ctx.parallel_program_path == NULL || ctx.serial_program_path == NULL) {
        log_error("Not all arguments was set");
        usage();
        exit(1);
    }
}

int main(int argc, char **argv) {
    parse_args(argc, argv);
#ifdef DEBUG
    set_log_level(LOG_DEBUG);
#endif // DEBUG

    struct timeval start = {0};
    struct timeval end = {0};
    pid_t pid;

    long elapsed_serial = 0;
    long elapsed_parallel = 0;

    gettimeofday(&start, NULL);

    pid = fork();
    fatal_if(pid < 0, "syscall fork error: %s", strerror(errno));

    if(pid == 0) {
        execlp("/bin/mpirun", "mpirun", ctx.parallel_program_path, NULL);
        log_fatal("syscall execlp error: %s", strerror(errno));
    } else {
        int status;
        waitpid(pid, &status, 0);

        gettimeofday(&end, NULL);
        elapsed_parallel = (end.tv_sec - start.tv_sec) * 1000;
        elapsed_parallel += (end.tv_usec - start.tv_usec) / 1000;

        log_info("Tempo di esecuzione programma parallelo: %ld ms\n", elapsed_parallel);
    }

    gettimeofday(&start, NULL);

    pid = fork();
    fatal_if(pid < 0, "syscall fork error: %s", strerror(errno));

    if(pid == 0) {
        execlp(ctx.serial_program_path, ctx.serial_program_path, NULL);
        log_fatal("syscall execlp error: %s", strerror(errno));
    } else {
        int status;
        waitpid(pid, &status, 0);

        gettimeofday(&end, NULL);
        elapsed_serial = (end.tv_sec - start.tv_sec) * 1000;
        elapsed_serial += (end.tv_usec - start.tv_usec) / 1000;

        log_info("Tempo di esecuzione programma sequenziale: %ld ms\n", elapsed_serial);
    }

    double speedup = (double)elapsed_serial/(double)elapsed_parallel;

    log_info("Speedup = %lf", speedup);

    return 0;
}