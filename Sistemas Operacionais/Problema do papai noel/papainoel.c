#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <syscall.h>

#define true 1
#define false 0

typedef sem_t sem;

int elfos = 0;
int renas = 0;

sem papaiNoelSem;
sem renaSem;

sem elfoTex;
sem multex;


int wait(sem *semaforo, char alguem[], char onde[]){
    printf("%s quer fazer ateracao em %s\n", alguem, onde);
    return sem_wait(semaforo);
}
int signal(sem *semaforo, int parametro, char alguem[], char onde[]){
    printf("%s liberou o caminho em %s\n", alguem, onde);
    for (int i = 0; i < parametro; i++) sem_post(semaforo);
    return 0; 
}

void prepararTreno(int id){
    printf("[THREAD Papai Noel %d] Preparando treno.....\n", id);
    sleep(500);
}
void ajudarElfos(int id){
    printf("[THREAD Papai Noel %d] Ajudando elfos.....\n", id);
    sleep(500);
}
void serAmarrada(int id, int renas_c){
    printf("[THREAD Rena %d, id:%d] Amarrando....\n", renas_c, id);
    sleep(500);
}
void pedirAjuda(int id, int elfo_c){
    printf("[THREAD Elfo %d, id:%d] Pedindo ajuda...\n",elfo_c, id);
    sleep(500);
}

void* papaiNoelThread(void *arg){                               /*dados do papai noel*/
    while (true){                                               /*repita para sempre*/
        int *threadId = (int *) arg;

        wait(&papaiNoelSem, "Papai noel", "Papai Noel SEM");    /*ele espera até que um elfo ou rena o sinalize)*/
        wait(&multex, "Papai Noel", "Multex");                  /*entra na região na crítica*/
        if (renas >= 9){                                        /*se houver nove renas esperando*/
            prepararTreno(*threadId);                           /*papai noel prepara o trenó*/

            /* No codigo do livro, o signal recebe 9 como
             * parametro, nao sei pq.
             * Vou deixar na funcao para fins futuros
             * */
            signal(&renaSem, 9, "Papai Noel", "Rena SEM");      /*sinaliza o semáforo das renas nove vezes*/ 
            renas -= 9;                                         /*as 9 renas são invocadas*/
        }else if (elfos == 3) ajudarElfos(*threadId), signal(&elfoTex, 1, "Papai Noel", "Elfo TEX"), elfos -=3;     /*se houver 3 elfos esperando, invoca papai noel para ajudar*/
        signal(&multex, 1, "Papai Noel", "Multex");             /*sinaliza que os 3 elfos foi ajudados e pode entrar mais*/
    }
}

void* renaThread(void *arg){                                    /*dados das renas*/
    while (true){                                               /*repita para sempre*/
        int *threadId = (int *) arg;
        wait(&multex, "Rena", "Multex");                        /*elas esperam até que o papai noel as sinalize para se engatilharem e após entra na região crítica*/
        renas++;                                                /*incrementam o contador de renas no buffer*/
        if (renas == 9) signal(&papaiNoelSem, 1, "Rena", "Papai Noel SEM");     /*se a 9º rena estiver pronta, ela sinaliza o papai noel e depois se junta aos outros que esperam*/
        signal(&multex, 1, "Rena", "Multe");

        wait(&renaSem, "Rena", "Rena SEM");
        serAmarrada(*threadId, renas);
    }
}


void* elfoThread(void *arg){
    while (true){
        int *threadInd = (int *)arg;


        wait(&elfoTex, "Elfo", "Elfo TEX");
        wait(&multex, "Elfo", "Multex");
        elfos++;
        if (elfos == 3) signal(&papaiNoelSem, 1, "Elfo", "Papai Noel SEM");
        else signal(&elfoTex, 1, "Elfo", "Elfo TEX");

        signal(&multex, 1, "Elfo", "Multex");

        pedirAjuda(*threadInd, elfos);

        wait(&multex, "Elfo", "Multex");
        elfos--;
        if (elfos == 0) signal(&elfoTex, 1, "Elfo", "Elfo TEX");
        signal(&multex, 1, "Elfo", "Multex");
    }
}


// Declaração de algumas constantes e apelidos

#define NELFOS 10
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

    pthread papaiNoel;
    pthread elfo[NELFOS];
    pthread rena[NRENAS];

    puts("Inicializando semaforos");
    sem_init(&papaiNoelSem, 0, 1);
    sem_init(&renaSem, 0, 1);
    sem_init(&elfoTex, 0, 1);
    sem_init(&multex, 0, 1);


    puts("Criando as Threads");
    puts("Em execução....");

    pthread_create(&papaiNoel, NULL, papaiNoelThread, (void *)&papaiNoel);
    for (int i = 0; i < NELFOS; i++) pthread_create(&elfo[i], NULL, elfoThread, (void *)&elfo[i]);

    for (int i = 0; i < NRENAS; i++) pthread_create(&rena[i], NULL, renaThread, (void *)&rena[i]);


    pthread_join(papaiNoel, NULL);
    for (int i = 0; i < NELFOS; i++) pthread_join(elfo[i], NULL);

    for (int i = 0; i < NRENAS; i++) pthread_join(rena[i], NULL);
    

    sem_destroy(&papaiNoelSem);
    sem_destroy(&renaSem);
    sem_destroy(&elfoTex);
    sem_destroy(&multex);

    return 0;
}
