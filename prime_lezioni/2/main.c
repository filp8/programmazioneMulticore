#include <stdio.h>

int main(int argc, char **argv) {
    
    for(char **it = argv; it < argv + argc ; ++it) {
        printf("%s\n", *it);
    }

    return 0;
}