#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int* parityMerge(int* arrayA, int* arrayB, int size) {
    int* newArray = (int*)malloc(sizeof(int) * size * 2);
    if (newArray == NULL) return NULL;

    int oddCounter = 0;
    int evenCounter = size*2 - 1;

    for(int i = 0; i < size; i++) {
        if(arrayA[i] % 2 != 0){
            newArray[oddCounter] = arrayA[i];
            oddCounter++;
        }else {
            newArray[evenCounter] = arrayA[i];
            evenCounter--;
        }

        if(arrayB[i] % 2 != 0){
            newArray[oddCounter] = arrayB[i];
            oddCounter++;
        }else {
            newArray[evenCounter] = arrayB[i];
            evenCounter--;
        }
    }

    return newArray;
}