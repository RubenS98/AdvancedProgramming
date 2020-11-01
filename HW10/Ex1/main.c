//
// Actividad 10 Ejercicio 1 - Concurrencia
// Programa que simula blanca nieves y los 7 enanitos
// 30 - oct - 2020
// Sabrina Santana A01025397
// Rubén Sánchez A01021759
//

/*
Análisis de los problemas de concurrencia
  - Problema con las sillas: Que un enanito vea que puede entrar pero antes de entrar le ganen el lugar
  - Problema con Blancanieves: que sirva más comidas de las necesarias por pensar que le tiene
  que servir a un enano al que ya le sirvió.
  - Problema con enanos: que más de un enano se fijé en la misma comida, peleen por el recurso y
  todos piensen que comieron aún cuando sólo uno lo hizo
Selección: Semáforos
Motivo: se decidió usar semáforos porque, además de que los enanos esperan y continuan cada
que el semáforo les indica (Blancanieves lo indica), el semáforo lleva un contador que nos
indica cuántas sillas están disponibles y cuántos platos de comida hay disponibles. Esto no
se podría lograr con las variables de condición ya que estás sólo pueden tener dos estados
(0 o 1) y los semáforos pueden tener más. Se uso mutex para proteger la variable global.
*/

#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define ENANOS 7//Número enanos
#define SILLAS 4//Numero sillas
#define NHILOS ENANOS + 1//Numero hilos

//Mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//Semáforos
sem_t silla, turno;
//Variable global
int esperando;

//Funciones
void * enanito(void * );
void * blancanieves(void * );

int main(int argc, const char * argv[])
{
    //Arreglo de hilos
    pthread_t * hilos = (pthread_t *) malloc (sizeof(pthread_t) * NHILOS);
    int result, i;
    pthread_t * aux;
    int indice = 0;

    //Inicializar semáforos
    sem_init(&silla, 0, SILLAS);
    sem_init(&turno, 0, 0);

    aux=hilos;

    //Crear blancanieves
    printf("Creando blancanieves \n");
    pthread_create(&aux, NULL, blancanieves, NULL);

    //Crear enanitos
    for (aux = hilos + 1; aux < (hilos+NHILOS); ++aux) {
        printf("Creando enanito %d\n", ++indice);
           pthread_create(aux, NULL, enanito, (void *) indice);
    }

    // Adjuntar todos los hilos
    for (aux = hilos; aux < (hilos+NHILOS); ++aux) {
        pthread_join(*aux, NULL);
    }

    //Liberar memoria
    free(hilos);

    //Destruir semáforos
    sem_destroy(&silla);
    sem_destroy(&turno);


    pthread_exit(NULL);
}

//Proceso blancanieves
void * blancanieves(void * arg) {

  //Infinito
  while(1){
    int value;
    //Obtener valor de semáforo silla
    sem_getvalue(&silla, &value);

    //Si hay 4 lugares disponibles, Blancanieves se va a pasear
    if(value == 4){
      printf("Blancanieves se va a pasear\n");
      sleep(10);
    } else {//Si hay enanos sentados
      //Proteger variable
      pthread_mutex_lock(&mutex);
      //Si hay enanitos sentados sin comida
      if(esperando>0){
        pthread_mutex_unlock(&mutex);
        //Servir comida
        printf("Blancanieves: Ya he servido la comida, puedes comenzar a comer\n");
        //Avisar que hay comida
        sem_post(&turno);
        sleep(1);
        pthread_mutex_lock(&mutex);
      }
      pthread_mutex_unlock(&mutex);

    }
  }
  pthread_exit(NULL);
};

//Proceso enanito
void *enanito(void * arg) {

  //ID enanito
  int indice = (int) arg;

  //Infinito
  while(1){

    //Enanito regresa de la mina
    printf("Enanito %d llega y ve si se puede sentar\n", indice);

    //Enanito espera a que haya lugar para sentarse
    sem_wait(&silla);

    //Enanito pide que le sirvan
    printf("Enanito %d se sienta y espera su turno\n", indice);

    //Aumentar variable para indicar que no le han servido y protegerla
    pthread_mutex_lock(&mutex);
    esperando+=1;
    pthread_mutex_unlock(&mutex);

    //Esperar a que haya comida
    sem_wait(&turno);

    //Decrementar variable para indicar que ya le sirvieron y protegerla
    pthread_mutex_lock(&mutex);
    esperando-=1;
    pthread_mutex_unlock(&mutex);

    //Enan empieza a comer
    printf("Enanito %d empieza a comer\n", indice);

    sleep(2);

    //Enano termina de comer y se regresa a la mina
    printf("Enanito %d acaba\n", indice);

    printf("Enanito %d vuelve a la mina\n", indice);

    //Enano desocupa silla
    sem_post(&silla);

    sleep(15);
  }
    pthread_exit(NULL);
}