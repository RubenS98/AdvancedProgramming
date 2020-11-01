//
// Actividad 9 - Concurrencia
// Programa que simula la doctrina “separados pero iguales implica desigualdad”
// 23 - oct - 2020
// Sabrina Santana A01025397
// Rubén Sánchez A01021759
//

#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define M 15 //Numero de mujeres a entrar
#define H 15 //Numero de hombres a entrar

//Mutex y condiciones
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t mujeres = PTHREAD_COND_INITIALIZER;
pthread_cond_t hombres = PTHREAD_COND_INITIALIZER;

int mujeres_adentro = 0;
int hombres_adentro = 0;

int gente_adentro = 0;

void * mujer_quiere_entrar(void *);
void * hombre_quiere_entrar(void *);
void * mujer_sale(void *);
void * hombre_sale(void *);

int main(int argc, const char * argv[]) {

    srand((int) time(NULL));

    int nhilos = 4;

    //Memoria dinámica de hilos
    pthread_t * hilos = (pthread_t *) malloc (sizeof(pthread_t) * nhilos);

    pthread_t * aux;
    aux = hilos;

    printf("Sanitario vacio \n");

    //Creación de hilos
    pthread_create(aux, NULL, mujer_quiere_entrar, NULL);

    pthread_create(aux+1, NULL, hombre_quiere_entrar, NULL);

    pthread_create(aux+2, NULL, mujer_sale, NULL);

    pthread_create(aux+3, NULL, hombre_sale, NULL);

    //Adjungtar todos los hilos
    for (pthread_t * aux = hilos; aux < (hilos+nhilos); ++aux) {
        pthread_join(*aux, NULL);
    }

    //Liberar memoria
    free(hilos);

    return 0;
}

void * mujer_quiere_entrar(void * id)
{
  int producidos = 0;
  int colaexterna = 0;

  //Mientras que falten mujeres por producir o haya mujeres esperando para entrar
  while (producidos < M || colaexterna > 0) {

      int randomnumber = rand() % 5 + 3; //random de 3 a 7

      //Proteger región crítica
      pthread_mutex_lock(&mutex);

      //Si no se han creado todas las mujeres crear una
      if(producidos < M){
        colaexterna++;
        printf("Llega mujer (%d en espera)\n", colaexterna);
        producidos++;
      }

      //Mujer puede entrar si hay mujeres adentro o si el sanitario está vacío
      if (gente_adentro == 0 || mujeres_adentro > 0) {
          colaexterna--;
          printf("--> Entra una mujer (%d en espera)\n", colaexterna);
          ++mujeres_adentro;

          //Indicar a thread mujer_sale que ya puede sacar mujeres
          if(mujeres_adentro == 1){
            printf("Sanitario ocupado por mujeres\n");
            pthread_cond_signal(&mujeres);
          }

          ++gente_adentro;
      }

      pthread_mutex_unlock(&mutex);
      //Simulación de cuanto tarda una mujer en llegar
      sleep(randomnumber);
    }
    pthread_exit(NULL);
}


void * hombre_quiere_entrar(void * id)
{
  int producidos = 0;
  int colaexterna = 0;
  //Mientras que falten homrbes por producir o haya hombres esperando para entrar
  while (producidos < H || colaexterna > 0) {

      int randomnumber = rand() % 5 + 3; //random de 3 a 7

      //Proteger región crítica
      pthread_mutex_lock(&mutex);

      //Si no se han creado todos los hombres crear uno
      if(producidos < H){
        colaexterna++;
        printf("Llega hombre (%d en espera)\n", colaexterna);
        producidos++;
      }

      //Hombre puede entrar si hay hombres adentro o si el sanitario está vacío
      if (gente_adentro == 0 || hombres_adentro > 0) {
          colaexterna--;
          printf("--> Entra un hombre (%d en espera)\n", colaexterna);
          ++hombres_adentro;

          //Indicar a thread hombre_sale que ya puede sacar mujeres
          if(hombres_adentro == 1){
            printf("Sanitario ocupado por hombres\n");
            pthread_cond_signal(&hombres);
          }

          ++gente_adentro;
      }

      pthread_mutex_unlock(&mutex);
      //Simulación de cuanto tarda un hombre en llegar
      sleep(randomnumber);
    }
    pthread_exit(NULL);
}

void * mujer_sale(void * id)
{
  int sacados = 0;

  //Mientras que falten mujeres por salir
  while (sacados < M) {

      int randomnumber = rand() % 5 + 3; //random de 3 a 7

      //Proteger región crítica
      pthread_mutex_lock(&mutex);

      //Si hay mujeres adentro sacar una mujer
      if(mujeres_adentro > 0){
        --mujeres_adentro;
        --gente_adentro;
        printf("Sale una mujer -->\n");
        sacados++;

        //Si no hay nadie adentro avisar a hombres que pueden entrar
        if (gente_adentro == 0) {
            printf("Sanitario vacio\n");
            pthread_cond_signal(&hombres);
        }
      } else {
        //Si no hay mujeres que sacar esperar a que una entre
        pthread_cond_wait(&mujeres, &mutex);
      }

      pthread_mutex_unlock(&mutex);
      //Simulación de cuanto tarda una mujer en salir
      sleep(randomnumber);
    }

    pthread_cond_signal(&hombres);
    pthread_exit(NULL);
}

void * hombre_sale(void * id)
{
  int sacados = 0;

  //Mientras que falten hombres por salir
  while (sacados < H) {

      int randomnumber = rand() % 5 + 3;  //random de 3 a 7

      //Proteger región crítica
      pthread_mutex_lock(&mutex);

      //Si hay hombres adentro sacar un hombre
      if(hombres_adentro > 0){
        --hombres_adentro;
        --gente_adentro;
        printf("Sale un hombre -->\n");
        sacados++;

        //Si no hay nadie adentro avisar a mujeres que pueden entrar
        if (gente_adentro == 0) {
            printf("Sanitario vacio\n");
            pthread_cond_signal(&mujeres);
        }
      } else{
        //Si no hay hombres que sacar esperar a que uno entre
        pthread_cond_wait(&hombres, &mutex);
      }

      pthread_mutex_unlock(&mutex);
      //Simulación de cuanto tarda un hombre en salir
      sleep(randomnumber);
    }

    pthread_cond_signal(&mujeres);
    pthread_exit(NULL);
}
