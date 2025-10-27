#include <stdio.h>
#include <stdlib.h>

int* mergeInt(int* A, int sizeA, int* B, int sizeB) {
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

float* mergeFloat(float* A, int sizeA, float* B, int sizeB) {
    float* C = (float*) malloc((sizeA + sizeB) * sizeof(float));

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
    int* C = mergeInt(A, 3, B, 3);
    for (int i = 0; i < 6; i++) {
        printf("%d ", C[i]);
    }
    free(C);

    float D[] = {1.2,3.45,17.4,23.4};
    float E[] = {2.3,4.5,13.564,22.3,77.4};
    float* F = mergeFloat(D,4,E,5);
    for (int i = 0; i < 9; i++) {
        printf("%f ", F[i]);
    }
    free(F);

    return 0;
}