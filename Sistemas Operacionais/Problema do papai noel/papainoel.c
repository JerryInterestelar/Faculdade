#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define true 1
#define false 0

typedef sem_t sem;

int elfos = 0;
int renas = 0;

sem papaiNoelSem;
sem renaSem;

sem elfoTex;
sem multex;


int wait(sem *semaforo){
    return sem_wait(semaforo);
}
int signal(sem *semaforo, int parametro){
    return sem_post(semaforo);    
}

void prepararTreno(){
    printf("Preparando treno.....\n");
    usleep(500);
}
void ajudarElfos(){
    printf("Ajudando elfos.....\n");
    usleep(500);
}
void serAmarrada(){
    printf("Amarrando....\n");
    usleep(500);
}
void pedirAjuda(){
    printf("Pedindo ajuda...\n");
    usleep(1000);
}

void* papaiNoelThread(void *arg){
    while (true){
        wait(&papaiNoelSem);
        wait(&multex);
        if (renas >= 9){
            prepararTreno();

            /* No codigo do livro, o signal recebe 9 como
             * parametro, nao sei pq.
             * Vou deixar na funcao para fins futuros
             * */
            signal(&renaSem, 9); 
            renas -= 9;
        }else if (elfos == 3) ajudarElfos();
        signal(&multex, 0);
    }
}

void* renaThread(void *arg){
    while (true){
        wait(&multex);
        renas++;
        if (renas == 9) signal(&papaiNoelSem, 0);
        signal(&multex, 0);

        wait(&renaSem);
        serAmarrada();
    }
}


void* elfoThread(void *arg){
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


// Declaração de algumas constantes e apelidos

#define NELFOS 50
#define NRENAS 9

typedef pthread_t pthread;

int main(){

    /*
     * Compilação com linkers
     * gcc papainoel.c -lpthread -lrt -D_REENTRANT -lpthread -o teste; ./teste
     *
     *
     *
     * */


    sem_init(&papaiNoelSem, 0, 1);
    sem_init(&renaSem, 0, 1);
    sem_init(&elfoTex, 0, 1);
    sem_init(&multex, 0, 1);

    pthread papaiNoel;
    pthread elfo[NELFOS];
    pthread rena[NRENAS];

    pthread_create(&papaiNoel, NULL, papaiNoelThread, NULL);
    for (int i = 0; i < NELFOS; i++) pthread_create(&elfo[i], NULL, elfoThread, NULL);

    for (int i = 0; i < NRENAS; i++) pthread_create(&rena[i], NULL, renaThread, NULL);


    pthread_join(papaiNoel, NULL);
    for (int i = 0; i < NELFOS; i++) pthread_join(elfo[i], NULL);

    for (int i = 0; i < NRENAS; i++) pthread_join(rena[i], NULL);
    
    sem_destroy(&papaiNoelSem);
    sem_destroy(&renaSem);
    sem_destroy(&elfoTex);
    sem_destroy(&multex);

    return 0;
}
