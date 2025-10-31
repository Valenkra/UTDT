#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/wait.h>


int main() {
    int rc = fork();
    if(rc < 0){
        return 1;
    }
    
    else if(rc == 0) { // solo se ejecuta el hijo
        printf("Hola soy Aureliano\n");
    
    } else { //solo se ejecuta el padre
        
        int rc1 = fork();
        
        if(rc < 0){
            return 1;
        }

        else if(rc1 == 0) { // solo se ejecuta el hijo
        
            printf("Hola soy Mica\n");
        
        } else{ //solo se ejecuta el padre
            
            int rc2 = fork();
        
            if(rc2 < 0){
                return 1;
            }
        
            else if(rc2 == 0) { // solo se ejecuta el hijo
        
                printf("Hola soy Valen\n");
        
            }else{ //solo se ejecuta el padre
        
                int rc3 = fork();
        
                if(rc3 < 0){
                    return 1;
                }
        
                else if(rc3 == 0) { // solo se ejecuta el hijo
                    printf("Hola soy Maie\n");
        
                } else { //solo se ejecuta el padre
                    
                    /* wait(NULL);
                    wait(NULL);
                    wait(NULL);
                    wait(NULL); */
                    int CANT_HIJOS = 4;
                    for(int i = 0; i < (CANT_HIJOS); i++) {
                        wait(NULL);
                    }

                    // Me asegure de que todos los hijos terminen sus procesos antes de hacer el printf
                    // Printea al final de todo
                    // No me puede asegurar que los demas hijos se ejecuten en orden
                    // Si me puede asegurar que se va a esperar a todos los hijos
                    printf("Mother\n");
                }
            }
        }
    }

    // QUIERO QUE EL PADRE VAYA ULTIMO - USO EL WAIT

    return 0;
}