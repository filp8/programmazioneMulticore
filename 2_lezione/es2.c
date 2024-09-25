#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "utils.h"

#define LENGHT 5

#define CHAR_TO_INT(c) (c - '0')

void print_int_array(int *array, int lenght) {
    for(int i = 0; i < lenght; ++i) {
        printf("%d", array[i]);
        if(i != lenght - 1)
            printf(", ");
    }
}

void read_char_array(char *array, int lenght) {
    if(read(STDIN_FILENO, array, lenght*sizeof(char)) == -1)
        eprintf("ERROR: %s", strerror(errno));
}

void read_int_array(int *array, int lenght) {
    char buf[lenght];

    memset(buf,0,lenght);

    if(read(STDIN_FILENO, buf, lenght*sizeof(char)) == -1)
        eprintf("ERROR: %s", strerror(errno));

    for(int i = 0; i < lenght; ++i)
         array[i] = CHAR_TO_INT(buf[i]);

}

int main(void) {
    int array1[LENGHT];
    int array2[LENGHT];

    
    read_int_array(array1, LENGHT);
    read_int_array(array2, LENGHT);

    for(int i = 0; i < LENGHT; ++i) {
        int x = array2[LENGHT - i - 1];
        array2[LENGHT - i - 1] = array1[i];
        array1[i] = x;
    }

    printf("Array 1: ");
    print_int_array(array1, LENGHT);
    putc('\n', stdout);
    printf("Array 2: ");
    print_int_array(array2, LENGHT);
    putc('\n', stdout);
}