// EJERCICIO 2
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    for(int i = 4; i>0; i--) {
        int a = fork();
        if(a > 0){
            printf("%i 1\n", i*2);
        }else {
            printf("%i 0\n", i*2);
        }
    }
    return 0;
}

/*
    ¿Què hace el codigo?
    En cada iteracion se hace un fork. En cada iteracion se duplica el proceso (porque se duplica "a")
    En total tenemos 2^4 = 16 procesos.
    Cada proceso imprime 4 veces (una por cada iteracion) un numero par (i
    *2) y un 1 si es padre o un 0 si es hijo.

    Se imprimen 30 líneas.

    En la primera iteracion imprime 2 a la 1 
    Despues 2 a la 2
    Despues 2 a la 3
    Despues 2 a la 4
    En total 30 lineas.

    Con i = 4:
    8 1
    8 0
    Con i = 3:
    6 1
    6 0
    Con i = 2:

    Punto 2:

    Para hacer que el proceso padre espere a que terminen todos sus hijos, se puede usar la función wait() en un bucle después del fork. Aquí hay una versión modificada del código que incluye esto:
    int main() {
    for(int i = 4; i>0; i--) {
        int a = fork();
        if(a > 0){
            wait(); // No hace falta que tenga el NULL
            printf("%i 1\n", i*2);
        }else {
            printf("%i 0\n", i*2);
        }
    }
        return 0;
    }

    Punto 3:
    int main() {
        int a = 1;
        for(int i = 2; i>0; i--) { // Como i empieza en dos, solo se vana crear dos hijos
            if(a > 0){
                int a = fork();
            }else {
                printf("%i 0\n", i*2);
            }
        }
    return 0;
}
*/