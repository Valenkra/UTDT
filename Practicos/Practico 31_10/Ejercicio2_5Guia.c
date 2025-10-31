#include <stdio.h>
#include <stdlib.h>



int main () {
    printf("Hello world (pid: %d)\n",(int)getpid());
    
    int h1 = fork();
    int h2 = fork();
    int h3 = fork();

    if(h1 == 0) {
        printf("Hello, I am child (pid:%d)\n", (int) getpid());
    }

    return 0;
}