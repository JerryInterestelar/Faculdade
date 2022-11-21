#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define true 1
#define false 0

typedef sem_t sem;

int elfos = 0;                  /* Numero de itens no Buffer*/
int renas = 0;

sem papaiNoelSem;

sem renaSem;                    /* Semaforos reservados para a relacao papai noel e rena/elfo*/
sem elfoSem;

sem elfoTex;
sem multex;


// Renomeação das funçoes de controle de semaforo de "Semaphore"
int wait(sem *semaforo){ 
    return sem_wait(semaforo);
}
int signal(sem *semaforo, int vezes){
    for (int i = 0; i < vezes; i++) sem_post(semaforo);
    return 0; 
}

// Funcoes de execução para fins de LOG (estas levam um segundo para serem executadas)
void prepararTreno(int id){
    printf("[THREAD Papai Noel %d] Preparando treno.....\n", id);
    sleep(1);
}
void ajudarElfos(int id){
    printf("[THREAD Papai Noel %d] Ajudando elfos.....\n", id);
    sleep(1);
}
void serAmarrada(int id){
    printf("[THREAD Rena: %d] Amarrando...\n", id);
    sleep(1);
}
void pedirAjuda(int id){
    printf("[THREAD Elfo: %d] Pedindo ajuda...\n", id);
    sleep(1);
}

void* papaiNoelThread(void *arg){               // Thread do papai noel
    while (true){                               // Ele executara esta rotina para sempre
        int *threadId = (int *) arg;                            

        wait(&papaiNoelSem);                    // Espera até que um elfo ou rena mande um sinal para ele
        wait(&multex);                          // Espera até conseguir acesso à região critica
                                                                
        if (renas == 9){                        // Se tiver nove renas esperando 
            prepararTreno(*threadId);           // Papai noel prepara o treno
            signal(&renaSem, 9);                // E sinaliza o semaforo das renas nove vezes
            renas = 0;                          // E zera o numero de renas esperando 
        }else if (elfos == 3) {
                                                // Se tiver tres elfos esperando
            ajudarElfos(*threadId);             // Papai noel irá ajuda-los
            signal(&elfoSem, 3);                // E sinaliza que mais tres elfos podem ser ficar esperando o atendimento
        }
        signal(&multex, 1);                     // Por fim libera a região critica para outra thread altera-la
    }
}

void* renaThread(void *arg){                    // Thread da Rena
                                                // Como a rena so vai ser amarrada uma vez, essa thread nao precisa
                                                // estar um um loop infinito

    int *threadId = (int *) arg;

    wait(&multex);                              // Espera para conseguir acesso a regiao critica
    renas++;                                    // incrementam o buffer de contagem de renas
    if (renas == 9)                             // Quando esta for a nona rena
        signal(&papaiNoelSem, 1);               // Sinaliza ao papai noel que todas as renas estao prontas para serem amarradas
    signal(&multex, 1);                         // Libera o acesso a regiao critica

    wait(&renaSem);                             // Agora, todas as renas aguardam o sinal para serem amarradas pelo papai noel
    serAmarrada(*threadId);                     // Quando liberadas pelo papai noel, estas são amarradas

    return 0;
}


void* elfoThread(void *arg){                    // Thread da rena
    while (true){                               // Repita para sempre
        int *threadId = (int *)arg;


        wait(&elfoTex);                         // Espera para conseguir acesso a fila de atendimento do papai noel
        wait(&multex);                          // Espera para conseguir acesso a regia critica
        elfos++;                                // incrementa o buffer da fila "elfos"
        if (elfos == 3)                         // Se ouver tres elfos na fila
            signal(&papaiNoelSem, 1);           // É sinalizado ao papai noel que ele precisa atender os elfos
                                                // e a fila de espera trava ate que estes sejam atendidos pela thread
                                                // do papai noel
        else 
            signal(&elfoTex, 1);                // Libera o acesso a fila para que mais um elfo possa entrar

        signal(&multex, 1);                     // Libera o acesso a regiao critica

        wait(&elfoSem);                         // Apenas o elfos que estiverem na fila poderam esperar pelo o atendimento do papai noel
        pedirAjuda(*threadId);                  // Procedimento de pedir ajuda

        wait(&multex);                          // Pede acesso a regiao critica mais uma vez para liberar o seu espaco no buffer de fila "elfos"
        elfos--;
        if (elfos == 0)                         // Quando nao tiver mais nenhum elfo na fila 
            signal(&elfoTex, 1);                // O ultimo elfo sinaliza que mais elfos podem entrar
        signal(&multex, 1);                     // Libera o acesso a regia critica

    }
}


// Declaração de algumas constantes e apelidos

#define NELFOS 20
#define NRENAS 9

typedef pthread_t pthread;


pthread papaiNoel;
pthread elfo[NELFOS];
pthread rena[NRENAS];

int main(){

    /*
     * Compilação e execução com linkers
     * gcc papainoel.c -lpthread -lrt -D_REENTRANT -lpthread -o teste; ./teste
     *
     * */

    puts("Inicializando semaforos");
    sem_init(&papaiNoelSem, 0, 0);
    sem_init(&renaSem, 0, 0);
    sem_init(&elfoSem, 0, 0);
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
    sem_destroy(&elfoSem);
    sem_destroy(&elfoTex);
    sem_destroy(&multex);

    return 0;
}
