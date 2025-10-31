#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/wait.h>

int p1(){
    printf(”helloworld(pid:%d)\n”,(int)getpid());
    int rc = fork();
    if(rc<0){
        //forkfailed;exit
        fprintf(stderr,”forkfailed\n”);
        exit(1);
    } elseif(rc==0){
    //child(newprocess)
        printf(”hello,Iamchild(pid:%d)\n”,(int)getpid());
    } else{
    //parentgoesdownthispath(originalprocess)
    printf(”hello,Iamparentof%d(pid:%d)\n”,rc,(int)getpid());
    }
    return 0;
}

int main(void){
    p1();
    //p2();Irhabilitandodeaunalasfunciones
    //p3();
    //p4();
    return 0;
}