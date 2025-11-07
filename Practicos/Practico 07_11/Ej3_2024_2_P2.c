/*
    Th1:
        - Lee a = 0
        - Suma a + 1
    -
    Th2:
        - Lee a = 0
        - Suma a + 1
    -
    Th3:
        - Lee a = 0
        - Suma a + 1
        - Guarda a = a + 1 (1)
    - 
    Th1:
        - Guarda a = a + 1 (1)
    -
    Th2:
        - Guarda a = a + 1 (1)


    Posibles respuestas;
        1. " 1 , 1 "
        2. " 3 , 3 "

    Rta: Agregarìa un lock con seccion crìtica en a = a + 1;
    y otro en b = b + 1;

    void f() {
        A();
        SECCION CRITICA
            a = a + 1;
        FIN SECCION CRITICA
        B();
        SECCION CRITICA
            b = b + 1;
        FIN SECCION CRITICA    
        
    }
*/

// Ejercicio 3:

void f() {
    A();
    // SECCION CRITICA - pthread_mutex_lock(&mutex_a);
    a = a + 1;
    // FIN DE SECCION CRITICA - pthread_mutex_unlock(&mutex_a);
    B();
    // SECCION CRITICA - pthread_mutex_lock(&mutex_b);
    b = b + 1;
    // FIN DE SECCION CRITICA - pthread_mutex_unlock(&mutex_b);
}
int a = 0;
int b = 0;

int main() {
    pthread_t th1, th2, th3;

    pthread_create(&th1, 0, &f, 0);.
    pthread_create(&th2, 0, &f, 0);
    pthread_create(&th3, 0, &f, 0);

    pthread_join(th1, 0);
    pthread_join(th2, 0);
    pthread_join(th3, 0);

    printf("a: %i, b: %i\n", a, b);
    return 0;
}

/*
    Respuesta al punto 4:
    Agrego una barrera entre A() y B()
        void f() {
            A();
            a = a + 1;
            BARRERA() - pthread_barrier_wait(&barrier);
            B();
            b = b + 1;

        
        }
    BARRERA: Primero se ejecuta todos los A en forma pararlela 
    y despues todos los B en forma paralelo. A siempre se va a ejecutar
    antes que B y B siempre se va a ejecutar despues que A.

    APRENDER SOBRE: Barrera, seaforo y seccion critica
*/