#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define true 1
#define false 0

typedef sem_t sem;

int elfos = 0;                                            /*numero de itens no buffer*/
int renas = 0;                                            /*numero de itens no buffer*/

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

void* papaiNoelThread(void *arg){                       /*dados do papai noel*/
    while (true){                                       /*repita para sempre*/
        wait(&papaiNoelSem);                            /*ele espera até que um elfo ou rena o sinalize)*/
        wait(&multex);                                  /*entra na região na crítica*/
        if (renas >= 9){                                /*se houver nove renas esperando*/
            prepararTreno();                            /*papai noel prepara o trenó*/
            signal(&renaSem, 9);                        /*e sinaliza o semáforo das renas nove vezes*/
            renas -= 9;                                 /*permitindo que as renas invoquem*/
        }else if (elfos == 3) ajudarElfos();            /*se houver 3 elfos esperando, invoca papai noel para ajudar*/
        signal(&multex, 0);                             /*mutex para evitar que elfos adicionais entrem enquanto três elfos estão sendo ajudados.*/
    }
}

void* renaThread(void *arg){                           /*dados das renas*/
    while (true){                                      /*repita para sempre*/
        wait(&multex);                                 /*elas esperam até que o papai noel as sinalize para se engatilharem e após entra na região crítica*/
        renas++;                                       /*incrementam o contador de renas no buffer*/
        if (renas == 9) signal(&papaiNoelSem, 0);      /*se a 9º rena estiver pronta, ela sinaliza o papai noel e depois se junta aos outros que esperam*/
        signal(&multex, 0);                            /*mutex para evitar que renas adicionais entrem após a 9° rena se juntar aos outros*/                        

        wait(&renaSem);                                /*renas esperam até serem amarradas */
        serAmarrada();                                 /*renas são amarradas */
    }
}


void* elfoThread(void *arg){                           /*dados dos elfos*/
    while (true){                                      /*repita para sempre*/
        wait(&elfoTex);                                /**/
        wait(&multex);                                 /**/
        elfos++;                                       /*incrementam o contador de elfos no buffer*/
        if (elfos == 3) signal(&papaiNoelSem, 0);      /*se houver 3 elfos esperando ajuda, eles sinaliza (acorda) o papai noel*/
        else signal(&elfoTex, 0);                      /*senão houver 3 elfos é sinalizado o elfoTex*/

        signal(&multex, 0);                            

        pedirAjuda();

        wait(&multex);
        elfos--;                                       /*decrementa o contador de elfos no buffer*/
        if (elfos == 0) signal(&elfoTex, 0);           /**/
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
