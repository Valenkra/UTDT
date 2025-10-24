// EJERCICIO 3.B GUIA 1
// POINTERS IN C
// POINTERS ABZ
#include <stdio.h>
#include <stdlib.h> // Para el malloc

// Sabemos nuestro size en boques
// Queremos saber el lenght de cada bloque
// Itero cada elemento de mi lista
// Para cada elemento le calculo su lenght
// Suma el lenght de cada palabra en el array a len_total

int getStrlen(char* s) {
    int len = 0;
    
    while (s[len] != '\0') { // Mientras sea distinto al caracter nulo (o sea el final) sigo
        len++;
    }

    return len;
}

//Tmb puedo escribir (char** v, int size) que es un doble puntero
char* superConcat(char* v[], int size) { 
    int len_total = 0;
    for(int i = 0; i<size; i++) {
        len_total += getStrlen(v[i]);
    }
    char* res = (char*) malloc(sizeof(char)*len_total+1);
    // sumamos un uno al final por el caracter nulo. NO olvidar!!
    int pos = 0;

    // Relleno el espacio que esa vacio y agarro todo los
    for(int i = 0; i<size; i++) { // Itero por palabra
        char* curr = v[i]; // Agarra los elementos del array original, es un temp
        int len = getStrlen(curr); // Calulo el lenght del curr para iterar sobre ese lenght
        for(int j = 0; j < len; j++){ // Guardo cada caracter del bloque que estoy iterando - Itero por caracter en palabra
            res[pos] = curr[j];
            pos++;
        }
    }
    res[pos] = '\0';
    return res;
}

int main() {
    // Declara el array primero
    char* palabras[3] = {"Eh_", "oo oo", "+Gol"};
    
    // Ahora llama a la función
    char* resultado = superConcat(palabras, 3);
    
    // Imprime el resultado
    printf("Resultado: %s\n", resultado);
    
    // Libera la memoria
    free(resultado);
    
    return 0;
}