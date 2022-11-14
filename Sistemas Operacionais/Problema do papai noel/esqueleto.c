#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define true 1
#define false 0

typedef int sem;

int elfos = 0;
int renas = 0;
sem papaiNoelSem = 0;
sem renaSem = 0;

sem elfoTex = 1;
sem multex = 1;


void wait(sem *semaforo);
void signal(sem *semaforo, int step);

void prepararTreno();
void ajudarElfos();
void serAmarrada();
void pedirAjuda();

void papaiNoel(){
    while (true){
        wait(&papaiNoelSem);
        wait(&multex);
        if (renas >= 9){
            prepararTreno();
            signal(&renaSem, 9);
            renas -= 9;
        }else if (elfos == 3) ajudarElfos();
        signal(&multex, 0);
    }
}

void rena(){
    while (true){
        wait(&multex);
        renas++;
        if (renas == 9) signal(&papaiNoelSem, 0);
        signal(&multex, 0);

        wait(&renaSem);
        serAmarrada();
    }
}


void elfo(){
    while (true){
        wait(&elfoTex);
        wait(&multex);
        elfos++;
        if (elfos == 3) signal(&papaiNoelSem, 0);
        else signal(&elfoTex, 0);

        signal(&multex, 0);

        pedirAjuda();

        wait(&multex);
        elfos--;
        if (elfos == 0) signal(&elfoTex, 0);
        signal(&multex, 0);
    }
}

int main(){

    printf("%d\n", elfoTex);

    return 0;
}
