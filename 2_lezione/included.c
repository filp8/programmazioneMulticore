#include <stdio.h>
#include <stdbool.h>

#define lenght(array) sizeof(array)/sizeof(*(array))

bool exists(int *array, int lenght, int to_search) {
    for(int *it=array; it < array + lenght; ++it) {
        if(*it == to_search)
            return true;
    }
    return false;
}

bool included(int *A, int *B, int n, int m) {
    for(int *it=A; it < A + n; ++it) {
        if(!exists(B,m,*it))
            return false;
    }
    return true;
}

int main(void) {
    int A[]  = {2, 1, 3, 2, 1};
    int B1[] = {5, 1, 3, 2};
    int B2[] = {5, 1, 3, 0};

    printf("%d\n",included(A,B1, lenght(A), lenght(B1)));
    printf("%d\n",included(A,B2, lenght(A), lenght(B2)));

}