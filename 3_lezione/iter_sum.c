#include <stdlib.h>
#include <stdio.h>

#include <time.h>
#include <assert.h>

int *generate_random_vector(size_t n_elements) {
    int *result = malloc(n_elements*sizeof(*result));
    assert(result != NULL);
    for(size_t i = 0; i < n_elements; i++) {
        result[i] = rand();
    }
    return result;
}

#define LENGTH (64*1024*1024)

int main(void) {
    srand(time(NULL));
    int *to_sum = generate_random_vector(LENGTH);
    size_t result = 0;
    for(int i = 0; i < LENGTH; i++) {
        result += to_sum[i];
    }
    free(to_sum);
    printf("result = %ld\n", result);
    return 0;
}