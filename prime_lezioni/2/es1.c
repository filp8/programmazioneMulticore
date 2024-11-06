#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "utils.h"

#define LENGHT 10

int main(void) {

    char array[LENGHT + 1] = {0};

    if(read(STDIN_FILENO, array, LENGHT) == -1) {
        eprintf("ERROR: %s", strerror(errno));
    }

    for(char *it = array; it < array + LENGHT; ++it) {
        *it = '*';
    }

    printf("%s\n", array);

    return 0;
}