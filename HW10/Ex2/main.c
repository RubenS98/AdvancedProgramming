//
// Actividad 10 Ejercicio 2 - Concurrencia
// Programa que simula compras de robots por secciones
// 30 - oct - 2020
// Sabrina Santana A01025397
// Rubén Sánchez A01021759
//

#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define SECCIONESGLOBAL 4 //Numero de secciones
#define PESOMAXIMO 8 //Peso maximo
#define ROBOTS 5 //Numero de robots

//Mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//Apuntador al inicio del arreglo de condiciones
pthread_cond_t * seccionescond;

//Variables globales
int * pesos;
int * secciones;

//Estructura de robot que incluye su peso actual y la seccion actual
typedef struct{
	int peso;
  int seccion;
} robotsstruct;

//Funcion
void * robot(void * );

int main(int argc, const char * argv[])
{
		//Arreglo de hilos
    pthread_t * hilos = (pthread_t *) malloc (sizeof(pthread_t) * ROBOTS);
		//Arreglo de secciones
    secciones = (int*) malloc (sizeof(int)*SECCIONESGLOBAL);
		//Arreglo de condicones que simulan cada seccion
    seccionescond = (pthread_cond_t*) malloc (sizeof(pthread_cond_t)*SECCIONESGLOBAL);

		//Variables locales
    pthread_t * aux;
    int * auxint;
    int indice = 0;


    //Crear y llenar arreglo de pesos extra de las secciones en 0's
    for (auxint = secciones; auxint < (secciones+SECCIONESGLOBAL); ++auxint) {
           * auxint = 0;
    }

    //Crear e inicializar 1 variable de condición por cada sección
    for (int j = 0; j < SECCIONESGLOBAL; ++j) {
           pthread_cond_t auxcond = PTHREAD_COND_INITIALIZER;
           *(seccionescond + j) = auxcond;
    }

    //Crear robots
    for (aux = hilos; aux < (hilos+ROBOTS); ++aux) {
        printf("Creando robot %d\n", ++indice);
        pthread_create(aux, NULL, robot, (void *) indice);
    }

    // Adjuntar todos los hilos
    for (aux = hilos; aux < (hilos+ROBOTS); ++aux) {
        pthread_join(*aux, NULL);
    }

		//Liberar memoria
    free(hilos);
    free(secciones);
    free(seccionescond);

    pthread_exit(NULL);
}

//Proceso robot
void *robot(void * arg) {

  //ID robot
  int indice = (int) arg;

	//Inicializar estructura que guarda la información del robot
  robotsstruct robotinfo;
  robotinfo.peso = rand() % 5 + 1;
	//Inicializar seccion actual en 0
  robotinfo.seccion = 0;

	//Ir de seccion en seccion
  while(robotinfo.seccion < SECCIONESGLOBAL){

		//Si el peso del robot no es mayor al peso máximo que aguanta la sección
    if(robotinfo.peso < PESOMAXIMO){

			//Proteger la región crítica
      pthread_mutex_lock(&mutex);

			//Mientras que el peso del robot más el peso en la seccion sea mayor al peso que aguanta cada seccion
      while(*(secciones + robotinfo.seccion) + robotinfo.peso > PESOMAXIMO){
				//El robot espera a entrar ya que la seccion no aguanta su peso
        printf("Robot %d esperando entrar a seccion %d (Peso: %d)\n", indice, robotinfo.seccion, *(secciones + robotinfo.seccion));
        pthread_cond_wait((seccionescond + robotinfo.seccion), &mutex);
      }

			//Una vez que puede entrar guardar su peso anterior
      int pesoanterior = robotinfo.peso;
			//Sumar peso del robot al peso de la seccion
      *(secciones + robotinfo.seccion) += robotinfo.peso;

      printf("Robot %d entra a la seccion %d (Peso: %d)\n", indice, robotinfo.seccion, *(secciones + robotinfo.seccion));

      pthread_mutex_unlock(&mutex);

			//Decidir si el robot va a comprar
      int randomcompra = rand() % 2;

			//Si el robot debe comprar indicar que compra y sumar un peso random del producto que compró
      if(randomcompra){
        printf("Robot %d comprando en la seccion %d\n", indice, robotinfo.seccion);
        int randompesoextra = rand() % 3 + 1;
        robotinfo.peso += randompesoextra;
        sleep(5);
      }

			//El robot tarda un poco en atravesar la seccion
      sleep(2);

			//Proteger la región crítica
      pthread_mutex_lock(&mutex);
			//Restar el peso original del robot de la seccion
      *(secciones + robotinfo.seccion) -= pesoanterior;
      printf("Robot %d sale de la seccion %d (Peso: %d)\n", indice, robotinfo.seccion, *(secciones + robotinfo.seccion));

			//Avisar a los robots que se encuentran esperando a entrar a la seccion que la seccion perdió peso
      pthread_cond_broadcast((seccionescond + robotinfo.seccion));

      pthread_mutex_unlock(&mutex);

			//Pasar a la siguiente seccion
      robotinfo.seccion++;

    } else {
			//Si la seccion no aguanta el puro peso de un robot significa que no puede seguir comprando
      printf("El robot %d ha sobrepasado el peso máximo que aguanta cada seccion por lo que no puede seguir.\n", indice);
			//Mandar al robot al final
			robotinfo.seccion = SECCIONESGLOBAL;
    }
}
    printf("Robot %d termina de comprar\n", indice);
    pthread_exit(NULL);
}