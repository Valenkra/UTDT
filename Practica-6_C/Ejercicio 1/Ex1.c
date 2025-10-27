#include <stdio.h>
#include <stdlib.h> // Para el malloc

int len(char* str) {
    int length = 0;
    // printf("El str vale %p \n",str);
    while (str[length] != '\0') {
        // printf("Str: %d -- ", str[length]);
        // printf("length vale: %d \n", length);
        length++;
    }
    return length;
}


char* copy(char* s) {
    int length = len(s); 
    char* newStr = (char*)malloc((length + 1) * sizeof(char)); // +1 para el caracter nulo

    /*
        ¿En què momento debo usar la funcion free()?
        Debo usar la función free() cuando ya no necesito usar la memoria que he asignado dinámicamente con malloc().
        Esto es importante para evitar fugas de memoria (memory leaks),
        que ocurren cuando se pierde la referencia a un bloque de memoria asignado dinámicamente sin liberarlo.
        En este caso, después de usar la cadena copiada (newStr),
        debería llamar a free(newStr) para liberar la memoria asignada.

    */

    for (int i = 0; i < length; i++) {
        newStr[i] = s[i]; // Copia cada caracter de s a newStr
        /* 
            newStr[i] es un puntero que apunta a la posición i de la cadena original
            s[i] es el caracter en la posición i de la cadena original

            ¿Pero entonces por qué funciona?
            Porque newStr[i] es una forma abreviada de escribir *(newStr + i)
            Y s[i] es una forma abreviada de escribir *(s + i)
            Entonces, al hacer newStr[i] = s[i], estamos copiando el valor del caracter en la posición i de s a la posición i de newStr.

            ¿Qué es un pointer y como puedo accederlo? 
            Un pointer es una variable que almacena la dirección de memoria de otra variable.
            Para acceder al valor al que apunta un pointer, usamos el operador de desreferenciación '*'.
            Por ejemplo, si tenemos un pointer 'p' que apunta a una variable 'x', podemos acceder al valor de 'x' usando '*p'.

            ¿Pero entonces a que apunta p? 
            p apunta a la dirección de memoria donde se encuentra almacenada la variable x.

        */
    }
    newStr[length] = '\0'; // Agregar el caracter nulo al final

    return newStr;
}

void replaceChar(char* s, char old, char new) {
    int i = 0;
    while (s[i] != '\0') {
        if (s[i] == old) {
            s[i] = new;
        }
        i++;
    }
}

char* concat(char* s1, char* s2) {
    int len1 = len(s1);
    int len2 = len(s2);

    char* newS = (char*)malloc((len1+len2)*sizeof(char));
    for(int i = 0; i < len1; i++) {
        newS[i] = s1[i];
    }
    for(int i = 0; i < len2; i++) {
        newS[i+len1] = s2[i];
    }
    newS[len1+len2] = '\0';
    return newS;

}

int main() {
    printf("%d\n",len("Hola"));
    printf("%d\n",len("Chau\n"));

    //ejecutar copy
    char* original = "Hola Mundo";
    char* copia = copy(original);
    printf("Original: %s\n", original);
    printf("Copia: %s\n", copia);  

    // Reemplazar caracteres
    replaceChar(copia, 'o', 'e');
    printf("Copia modificada: %s\n", copia);


    char* nuevo = concat(original, copia);
    printf("Copia modificada: %s\n", nuevo);


    // Liberar memoria
    free(copia);
    free(nuevo);
    return 0;
}

