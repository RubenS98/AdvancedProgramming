//
// Actividad 8 - Concurrencia
// Program that simulates bank operations using synchronization mechanisms
// 16 - oct - 2020
// Sabrina Santana A01025397
// Rubén Sánchez A01021759
//

#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define BSIZE 5 //CAJEROS GENERALES
#define BSIZE2 3 //CAJEROS EMPRESARIALES
#define N 100 //CLIENTES GENERALES
#define E 50//CLIENTES EMPRESARIALES
#define CONSUMIDORES 8 //NUMERO DE CAJEROS

int elementos[BSIZE]; //BUFFER GENERALES
int elementos2[BSIZE2]; //BUFFER EMPRESARIALES

int out = 0; //OUT GENERALES
int out2 = 0; //OUT EMPRESARIALES

int total = 0; //TOTAL GENERALES
int total2 = 0; //TOTAL EMPRESARIALES

int consumidos = 0; //CONSUMIDOS GENERALES
int consumidos2 = 0; //CONSUMIDOS EMPRESARIALES

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //MUTEX GENERALES Y EMPRESARIALES
//CONDICIONES GENERALES
pthread_cond_t consume_t = PTHREAD_COND_INITIALIZER;
pthread_cond_t produce_t = PTHREAD_COND_INITIALIZER;
//CONDICIONES EMPRESARIALES
pthread_cond_t consume_t2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t produce_t2 = PTHREAD_COND_INITIALIZER;

void * clientegeneral(void *);
void * clienteempresarial(void *);
void * cajerogeneral(void *);
void * cajeroempresarial(void *);

int main(int argc, const char * argv[]) {

    srand((int) time(NULL));

    int nhilos = 10;

    //Arreglo con hilos, incluye cajeros y agentes
    pthread_t * cajeros = (pthread_t *) malloc (sizeof(pthread_t) * nhilos);

    int indice = 0;

    pthread_t * aux;

    int difcajero = 0;

    //FOR para crear los hilos cajeros
    for (aux = cajeros; aux < (cajeros+nhilos-2); ++aux) {
        printf("(I) Creando cajeros %d\n", ++indice);
        if(difcajero<5){
           pthread_create(aux, NULL, cajerogeneral, (void *) indice);
        } else {
            pthread_create(aux, NULL, cajeroempresarial, (void *) indice);
        }
        difcajero++;
    }

    //Crear agente general
     printf("(I) Creando el agente GENERAL %d\n", ++indice);
     pthread_create(aux, NULL, clientegeneral, (void *) indice);
     aux++;
    //Crear agente empresarial
     printf("(I) Creando el agente EMPRESARIAL %d\n", ++indice);
     pthread_create(aux, NULL, clienteempresarial, (void *) indice);

    // Adjuntar todos los hilos
    for (aux = cajeros; aux < (cajeros+nhilos); ++aux) {
        pthread_join(*aux, NULL);
    }

    //Liberar hilos
    free(cajeros);

    return 0;
}

void * clientegeneral(void * arg)
{
    int id = (int) arg;
    int in = 0;
    int producidos = 0;

    printf("(P) Inicia cliente %d\n", id);

    //Mientras haya clientes generales a producir
    while (producidos < N) {
          int randomnumber = rand() % (22-5+1)+5;
          //Proteger region critica
          pthread_mutex_lock(&mutex);

          //Checar que haya espacio en el buffer
          if (total < BSIZE && producidos < N) {

              elementos[in] = producidos;

              printf("+++ (Cliente tipo general) Id: %d\n", elementos[in]);

              ++producidos;

              ++in;
              in %= BSIZE;
              ++total;

              //Si existe un cliente general que atender, despertar a los cajeros
              if (total == 1) {
                  pthread_cond_broadcast(&consume_t);
              }
          }
          else if(producidos<N){
              // El buffer está lleno, se va a dormir
              printf("-------------- Cliente general %d espera en la fila ------------\n", elementos[in]);
              pthread_cond_wait(&produce_t, &mutex);
              printf("-------------- Cliente general %d es atendido -----------\n", elementos[in]);
          }

          pthread_mutex_unlock(&mutex);
          sleep(randomnumber);
        }
    pthread_exit(NULL);
}

void * clienteempresarial(void * arg)
{
    int id = (int) arg;
    int in2 = 0;
    int producidosemp = 0;

    printf("(P) Inicia cliente emp %d\n", id);

    //Mientras haya empresariales a producir
    while (producidosemp < E) {
          int randomnumber = rand() % (34-9+1)+9;

          //Proteger region critica
          pthread_mutex_lock(&mutex);

          //Checar que haya espacio en el buffer
          if (total2 < BSIZE2 && producidosemp < E) {

              elementos2[in2] = producidosemp;

              printf("+++ (Cliente tipo empresarial) Id: %d\n",elementos2[in2]);

              ++producidosemp;

              ++in2;
              in2 %= BSIZE2;
              ++total2;

              //Si existe un cliente empresarial que atender, despertar a los cajeros
              if (total2 == 1) {
                  pthread_cond_broadcast(&consume_t2);
              }
          }
          else if(producidosemp<E){
              // El buffer está lleno, se va a dormir
              printf("-------------- Cliente empresarial %d espera en la fila ------------\n", elementos2[in2]);
              pthread_cond_wait(&produce_t2, &mutex);
              printf("-------------- Cliente empresarial %d es atendido -----------\n", elementos2[in2]);
          }

          pthread_mutex_unlock(&mutex);
          sleep(randomnumber);
        }
    pthread_exit(NULL);
}


void * cajerogeneral(void * arg)
{
    int id = (int) arg;
    int cont = 0;

    printf("(C) Inicia cajero general %d\n", id);

    //Mientras que haya clientes generales que atender
    while (consumidos < N) {
        int randomnumber = rand() % (5-3+1)+3;
        //Proteger region critica
        pthread_mutex_lock(&mutex);

        //Checar que haya clientes generales en la fila
        if(consumidos < N && total > 0){
              printf("--- (Id Cajero general %d) Atendiendo a cliente general: %d\n", id, elementos[out]);

              ++cont;

              ++consumidos;

              ++out;
              out %= BSIZE;
              --total;

              //Si hay algun espacio en la fila, despertar a los clientes
              if (total == (BSIZE-1)) {
                  pthread_cond_broadcast(&produce_t);
              }

          else if(consumidos < N){
              // El buffer está vacío, se va a dormir
              printf("-------------- Cajero general %d se durmió ------------\n", id);
              pthread_cond_wait(&consume_t, &mutex);
              printf("-------------- Cajero general %d se despertó ------------\n", id);
          }
        }
        pthread_mutex_unlock(&mutex);
        sleep(randomnumber);

        //Cada vez que atiende a 5 clientes descansa 3 segundos
        if(cont>=5){
          printf("--- (Id Cajero general %d) Descanso de 3 segundos\n", id);
          sleep(3);
          cont = 0;
        }

    }

    printf("Cajero general %d terminó\n", id);
    pthread_cond_broadcast(&consume_t);
    pthread_exit(NULL);
}

void * cajeroempresarial(void * arg)
{
    int id = (int) arg;
    int cont2 = 0;

    printf("(C) Inicia cajero empresarial %d\n", id);

    //Mientras que haya empresariales que atender
    while (consumidos2 < E) {
        //Proteger region critica
        pthread_mutex_lock(&mutex);

        //Checar que haya clientes empresariales en la fila
        if(consumidos2 < E && total2 > 0){
              printf("--- (Id Cajero empresarial %d) Atendiendo a cliente empresarial: %d\n", id, elementos2[out2]);

              ++cont2;

              ++consumidos2;

              ++out2;
              out2 %= BSIZE2;
              --total2;

              //Si hay algun espacio en la fila, despertar a los clientes
              if (total2 == ( BSIZE2 - 1)) {
                  pthread_cond_broadcast(&produce_t2);
              }
        //Si no hay empresariales que atender en la fila checar que haya clientes generales
        } else if (total > 0){
              printf("--- (Id Cajero empresarial %d) Atendiendo a cliente general: %d\n", id, elementos[out]);

              ++cont2;

              ++consumidos;

              ++out;
              out %= BSIZE;
              --total;

              //Si hay algun espacio en la fila, despertar a los clientes
              if (total == (BSIZE-1)) {
                  pthread_cond_broadcast(&produce_t);
              }
        } else if(consumidos < N){
            // El buffer está vacío, se va a dormir
            printf("-------------- Cajero general %d se durmió ------------\n", id);
            pthread_cond_wait(&consume_t2, &mutex);
            printf("-------------- Cajero general %d se despertó ------------\n", id);

        }

        pthread_mutex_unlock(&mutex);

        //Cada vez que atiende a 5 clientes descansa 3 segundos
        if(cont2>=5){
          printf("--- (Id Cajero EMP %d) Descanso de 3 segundos\n", id);
          sleep(3);
          cont2 = 0;
        }

    }

    printf("Cajero emp %d terminó\n", id);
    pthread_cond_broadcast(&consume_t2);
    pthread_exit(NULL);
}