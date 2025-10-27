#include <stdio.h>
#include <stdlib.h>

int* merge(int* A, int sizeA, int* B, int sizeB) {
    int* C = (int*) malloc((sizeA + sizeB) * sizeof(int));

    int tempA = 0;
    int tempB = 0;
    int i = 0;

    while (i < (sizeA + sizeB)) {
        if((tempA < sizeA && A[tempA] < B[tempB]) || (tempB == sizeB)){
            C[i] = A[tempA];
            tempA++;
        }
        else if((tempB < sizeB && B[tempB] <= A[tempA]) || (tempA == sizeA)){
            C[i] = B[tempB];
            tempB++;
        }
        i++;
    }

    return C;
}

int main() {
    int A[] = {1, 2, 5};
    int B[] = {3, 4, 6};
    int* C = merge(A, 3, B, 3);
    for (int i = 0; i < 6; i++) {
        printf("%d ", C[i]);
    }
    free(C);
    return 0;
}