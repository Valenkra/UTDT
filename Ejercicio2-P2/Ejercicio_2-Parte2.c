#include <stdio.h>
#include <stdlib.h>

struct list {
    struct node* first;
    int size;
};


struct node {
    struct node* next;
    int data;
};

/*
    En base a estas estructuras se construye el siguiente ejemplo
    donde el puntero a twoLists es un arreglo de dos elementos de 
    tipo struct list. La primer lista del ejemplo tiene 4 datos,
    mientras uq el asegunda lista tiene 3 datos.

    Se pide implementar las siguientes funciones:

    struct list* getMerge(struct list* twoLists)

    Toma un puntero a un arreglo de dos listas e intercala los elementos
    de ambas listas en una nueva lista. Comienza por el primer elemento
    de la primera lista, y luego continua con el primer elemento de la 
    segunda lista, intercalando uno a uno. Las listas pueden ser de
    distinto tamanio, quedando la lista mas larga sin intercalar.
    La funcion debe borrar el arreglo de listas pasado por parametro
    y retornar una nueva lista

*/

struct list* getMerge(struct list* twoLists) {
    struct node *p1 = twoLists[0].first; // Accedo a la primera lista
    struct node *p2 = twoLists[1].first; // Accedo a la segunda lista

    
}

int main() {
    // Crear arreglo de dos listas
    struct list twoLists[2];
    
    // Crear nodos para la primera lista (4 elementos)
    struct node* n1 = (struct node*)malloc(sizeof(struct node));
    struct node* n2 = (struct node*)malloc(sizeof(struct node));
    struct node* n3 = (struct node*)malloc(sizeof(struct node));
    struct node* n4 = (struct node*)malloc(sizeof(struct node));
    struct node* n5 = (struct node*)malloc(sizeof(struct node));
    
    n1->data = 10;
    n1->next = n2;
    n2->data = 20;
    n2->next = n3;
    n3->data = 30;
    n3->next = n4;
    n4->data = 40;
    n4->next = n5;
    n5->data = 50;
    n5->next = NULL;
    
    twoLists[0].first = n1;
    twoLists[0].size = 5;
    
    // Crear nodos para la segunda lista (3 elementos)
    struct node* m1 = (struct node*)malloc(sizeof(struct node));
    struct node* m2 = (struct node*)malloc(sizeof(struct node));
    struct node* m3 = (struct node*)malloc(sizeof(struct node));
    
    m1->data = 100;
    m1->next = m2;
    m2->data = 200;
    m2->next = m3;
    m3->data = 300;
    m3->next = NULL;
    
    twoLists[1].first = m1;
    twoLists[1].size = 3;
    
    // Llamar a la función
    struct node* lastA = NULL;
    struct node* lastB = NULL;
    
    getMerge(twoLists);
    
    return 0;
}