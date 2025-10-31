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
    // Son dos contextos iguales pero estan copiados asi que no se comunican entre si
    // Uso un wait para esperar a que llegue el hijo antes de que muera el padre
    // Si el padre muere, mueren todos los hijos
    
    int rc = fork(); // Lo uso para dividir para que una cosa la ejecute el padre y otra cosa la ejecute el hijo
    
    // Es el mismo proceso pero esta duplicado
    
    if(rc < 0){ // Si el fork da menor a cero, no se pudo crear el hijo
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

int p2(int argc,char* argv[]){
    printf("helloworld(pid:%d)\n",(int)getpid());
    int rc = fork();
    if(rc < 0){
        //forkfailed;exit
        fprintf(stderr,"forkfailed\n");
        exit(1);
    }else if(rc == 0){
        //child(newprocess)
        printf("hello, I am child ( pid: %d )\n",(int)getpid());
        sleep(1); // Nos asegura que el padre llegue al wait
    } else{
        //parentgoesdownthispath(originalprocess)
        int wc = wait(NULL); // El padre llega al wait y agarra el id del hijo que llegue primero
        printf("hello, I am parent of %d ( wc: %d )( pid: %d )\n", // Una vez que lo recolectò imprime primero el hijo y despues el padre
        rc,wc,(int)getpid());
    }
    return 0;
}

int p3(){
    printf("helloworld(pid:%d)\n",(int)getpid());
    int rc=fork();

    if(rc < 0){

    //forkfailed;exit
    fprintf(stderr,"forkfailed\n");
    exit(1);

    }else if(rc==0){

        //child(newprocess)
        printf("hello,Iamchild(pid:%d)\n",(int)getpid());
        char*myargs[3];
        myargs[0]=strdup("wc");//program:"wc"(wordcount) Hace este programa
        myargs[1]=strdup("p3.c");//argument:filetocount Hace el programa con este archivo
        myargs[2]=NULL;//marksendofarray
        execvp(myargs[0],myargs);//runswordcount "Deja de hacer lo que estas haciendo y se pone a contar las palabras del otro archivo"
        printf("thisshouldn'tprintout");

    }else{
        //parentgoesdownthispath(originalprocess)
        int wc = wait(NULL); // El padre espera al hijo
        printf("hello,Iamparentof%d(wc:%d)(pid:%d)\n", // 
        rc,wc,(int)getpid());
    }
    return 0;
}


int p4(int argc,char*argv[]){ // A diferencia del tres, aca guardan la info en un archivo
    int rc=fork();
    if(rc<0){

        //forkfailed;exit
        fprintf(stderr,"forkfailed\n");
        exit(1);
    
    } else if(rc==0){
    
        //child:redirectstandardoutputtoafile
        close(STDOUT_FILENO);
        open("./p4.output", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
        //nowexec"wc"...
        char*myargs[3]; // guarda las palabras en el archivo
        myargs[0]=strdup("wc");//program:"wc"(wordcount)
        myargs[1]=strdup("p4.c");//argument:filetocount
        myargs[2]=NULL;//marksendofarray
        execvp(myargs[0],myargs);//runswordcount
    
    }else{
    
        //parentgoesdownthispath(originalprocess)
        int wc = wait(NULL);
    
        assert(wc >= 0); // chequeo de error. Si da -1 se rompio algo
    }

    return 0;
}

// Sleep(1) manda a dormir al hijo un segundo asì el padre llega bien al wait

int main(void){
    //p1();
    //p2(); //Ir habilitando de a una las funciones
    p3();
    //p4();
    return 0;
}