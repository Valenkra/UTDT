// EJERCICIO 2
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    for(int i = 4; i>0; i++) {
        int a = fork();
        if(a > 0){
            printf("%i 1\n", i*2);
        }else {
            printf("%i 0\n", i*2);
        }
    }
    return 0;
}