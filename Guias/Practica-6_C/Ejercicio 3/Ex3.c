#include <stdlib.h>
#include <stdio.h>

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

char* longest(char* v[], int size) {
    int index = 0;
    for (int i = 0; i < size; i++) {
        if(len(v[i]) > len(v[index])) {
            index = i;
        }
    }
    return v[index];
}

int main () {
    char* myList[4] = {"o","Alohaaaa","Pepe","Eh"};
    printf("The longest is: %s \n",longest(myList, 4));
    printf("Mi Cuenta 0x%x \n", (0x12+0x33));
    return 0;
}