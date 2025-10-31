#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/wait.h>

int p1(){
    printf("Hello world (pid: %d)\n",(int)getpid());
    // Fork: Copia de toda la memoria y proceso
    // Si yo modifico el fork el original no se da cuenta
    
    int rc = fork(); // Lo uso para dividir para que una cosa la ejecute el padre y otra cosa la ejecute el hijo
    
    // Es el mismo proceso pero esta duplicado
    
    if(rc < 0){
        // fork failed; exit - Fork duplica el proceso (ejecuto dos procesos iguales)
        // Se crea un proceso padre y un proceso hijo
        // Devuelve un valor para entender si esas ejecutando el proceso padre y el proceso hijo
        // Devuelve los dos valores (Padre e Hijo)
        // El padre suele recibir un valor mayor a cero que es el identificador del hijo
        // El hijo recibe cero

        fprintf(stderr,"fork failed\n");
        exit(1);

    } else if(rc == 0){ // Aca entra el hijo --> rc == 0

        //child (newprocess)
        printf("Hello, I am child (pid:%d)\n", (int) getpid());

    } else{ // Aca entra el padre --> rc > 0

        //parent goes down this path (original process)
        printf("Hello, I am parent of %d (pid: %d)\n", rc, (int) getpid());

    }
    return 0;
}

int main(void){
    p1();
    //p2(); Ir habilitando de a una las funciones
    //p3();
    //p4();
    return 0;
}