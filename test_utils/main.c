#define STRINGS_IMPLEMENTATION
#define ARENA_IMPLEMENTATION
#include "utils/common.h"
#include "utils/strings.h"
#include "utils/arena.h"

#include <stdio.h>

void test_normal_alloc() {
    Cstr *ciao = "ciao";
    size_t len = strlen(ciao) + 1;
    char *new_ciao = malloc(len);
    char *new_ciao1 = malloc(len);
    new_ciao1[0] = 'x';
    new_ciao1[1] = '\0';
    char *new_ciao2 = malloc(len);
    new_ciao2[0] = 'y';
    new_ciao2[1] = '\0';
    memcpy((void*)new_ciao, ciao, len);
    printf("%s\n", new_ciao);
    printf("%s\n", new_ciao1);
    printf("%s\n", new_ciao2);
    free(new_ciao);
    free(new_ciao1);
    free(new_ciao2);
}

void test_arena_alloc() {
    Cstr *ciao = "ciao";
    size_t len = strlen(ciao) + 1;
    Arena a = {0};
    Cstr *new_ciao = arena_alloc(&a, len);
    char *new_ciao1 = arena_alloc(&a, len);
    *new_ciao1 = 'x';
    char *new_ciao2 = arena_alloc(&a, len);
    *new_ciao2 = 'y';
    memcpy((void*)new_ciao, ciao, len);
    printf("%s\n", new_ciao);
    printf("%s\n", new_ciao1);
    printf("%s\n", new_ciao2);
    arena_free(&a);
}

int main(void) {
    //test_arena_alloc();
    //test_normal_alloc();
}