/*
  Proyecto final programación avanzada
  main.c
  multiplicacion paralela de matrices

  Equipo 5
  Rubén Sánchez A01021759
  Octavio Garduza A01021660
  Christian Dalma A01423166
  Sabrina Santana A01025397

  Implementación paralela en 4 nodos del algoritmo de multiplicación de matrices
  estándar utilizando OpenMP y MPI.
*/

#include "mpi.h"
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

/*
 * Función: transpose
 * --------------------
 * transpone matriz representada en arreglo lineal
 *
 *  a: apuntador al elemento inicial de la matriz a transponer
 *  b: apuntador al elemento inicial del arreglo donde se guardará a transpuesta
 *  rows: número de filas de matriz a
 *  cols: número de columnas de matriz a
 *
 */
void transpose(int * a, int * b, int rows, int cols)
{
  int i, j;
  #pragma omp for collapse(2)
  for (i = 0; i < rows; i++)
    for (j = 0; j < cols; j++)
      *(b + rows*j + i) = *(a + cols*i + j);
}

/*
 * Función: printMatrix
 * --------------------
 * imprime los elementos de un arreglo en formato de matriz
 *
 *  matrix: apuntador a elemento inicial de la matriz a imprimir
 *  rows: número de filas de matriz
 *  cols: número de columnas de matriz
 *
 */
void printMatrix(int * matrix, int rows, int cols)
{
  int i, j;
  printf("[\n");
  #pragma omp for
  for (i = 0; i < rows; i++){
    printf("   [ ");
    for (j = 0; j < cols; j++){
      printf("%d ", *(matrix + cols*i + j));
    }
    printf("]\n");
  }
  printf("]\n");
}

int main(int argc, char *argv[])
{
    //Declaración de variables
    int myid, numprocs, nh, tid, length, i, colPerProc, myresult, disp;
    int remainder, limit;
    int rows1, cols1, rows2, cols2;
    int seed = time(NULL);
    double startTotal, startMult, stop;
    char hostname[MPI_MAX_PROCESSOR_NAME];
    int * matrix1, * matrix2, * gatheredMatrix, * colsForProc, * matrix2Transposed;
    int * finalMatrix, * partialFinalMatrix;
    int * scatterSendCounts, * gatherSendCounts, * scatterDisp, * gatherDisp;
    int * rowscols = (int *) malloc(4 * sizeof(int));

    //Inicialización de entorno mpi
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);

    MPI_Get_processor_name(hostname, &length);

    //Pedir dimensiones de las matrices al usuario
    if(myid==0){
      printf("Bienvenido al sistema de planificación urbana.\n");

      printf("Introduzca cuantas filas tiene la matriz 1:\n");
      scanf("%d", (rowscols));
      printf("Introduzca cuantas columnas tiene la matriz 1:\n");
      scanf("%d", (rowscols+1));
      printf("Introduzca cuantas filas tiene la matriz 2:\n");
      scanf("%d", (rowscols+2));
      printf("Introduzca cuantas columnas tiene la matriz 2:\n");
      scanf("%d", (rowscols+3));
    }

    //Todos los procesos esperan el input de los datos
    MPI_Barrier(MPI_COMM_WORLD);

    //Filas y columnas de las matrices enviadas a todos los procesos
    MPI_Bcast(rowscols, 4, MPI_INT, 0, MPI_COMM_WORLD);

    //Valores de filas y columnas se asignan a las variables correspondientes
    rows1 = *(rowscols);
    cols1 = *(rowscols+1);
    rows2 = *(rowscols+2);
    cols2 = *(rowscols+3);

    //Verificar que las matrices se puedan multiplicar en el nodo master
    //Si no se pueden, terminar programa
    if(myid==0 && cols1 != rows2){
      printf("Las matrices no se pueden multiplicar.\n");
      return 0;
    }

    //Si no se pueden multiplicar las matrices, terminar programa en cada procesador
    if(cols1 != rows2){
      return 0;
    }

    //Asignar memoria
    matrix1 = (int *) malloc(rows1 * cols1 * sizeof(int));
    matrix2 = (int *) malloc(rows2 * cols2 * sizeof(int));
    gatheredMatrix = (int *) malloc(rows1 * cols2 * sizeof(int));
    finalMatrix = (int *) malloc(rows1 * cols2 * sizeof(int));
    matrix2Transposed = (int *) malloc(cols2 * rows2 * sizeof(int));

    startTotal = MPI_Wtime();

    //En el nodo master
    if (myid == 0)
    {
          srand(seed);
          //Inicializar datos en paralelo con OpenMP
          #pragma omp parallel for private(i) shared(matrix1, rows1, cols1)
            //Crear matriz 1 con datos aleatorios
            for(i = 0; i < rows1*cols1; i++) {
              *(matrix1 + i) = rand() % 500 + 100;
            }

          //Inicializar datos en paralelo con OpenMP
          #pragma omp parallel for private(i) shared(matrix2, rows2, cols2)
            //Crear matriz 2 con datos aleatorios
            for(i = 0; i < rows2*cols2; i++) {
              *(matrix2 + i) = rand() % 500 + 100;
            }

          //Imprimir matrices creadas
          printf("\n");
          printf("Matrix 1\n");
          printMatrix(matrix1, rows1, cols1);
          printf("\n");
          printf("Matrix 2\n");
          printf("\n");
          printMatrix(matrix2, rows2, cols2);

          //Empezar a contar tiempo de procesamiento
          startMult = MPI_Wtime();

          //Transponer matriz 2
          transpose(matrix2, matrix2Transposed, rows2, cols2);

          printf("\n");
    }

    //Compartir matriz 1 con todos los procesos
    MPI_Bcast(matrix1, rows1*cols1, MPI_INT, 0, MPI_COMM_WORLD);

    //Obtener cuantas columnas va a procesar cada procesador
    colPerProc = rows2/numprocs + 1;

    //Elementos de la matriz 2 a enviar a cada proceso
    scatterSendCounts = (int *) malloc(numprocs * sizeof(int));
    //Elementos de la matriz final que regresa cada proceso
    gatherSendCounts = (int *) malloc(numprocs * sizeof(int));
    //Intervalo según el cual se va a dividir la matriz 2 al enviarla
    //a los procesos
    scatterDisp = (int *) malloc(numprocs * sizeof(int));
    //Intervalo según el cual se van a obtener las secciones de la matriz
    //final en el nodo master
    gatherDisp = (int *) malloc(numprocs * sizeof(int));

    //Nodo master
    if (myid == 0)
    {
        //Primer intervalo es 0 porque es donde empieza el arreglo
        *(scatterDisp) = 0;
        *(gatherDisp) = 0;

        //Elementos de la matriz 2 que faltan por asignar a un proceso
        remainder = cols2*rows2;
        //Booleano para saber si ya no se pueden repartir más elementos de la matriz 2
        limit = 0;

        #pragma omp for
          //Llenar scatterSendCounts con número de elementos
          //que serán enviados a cada proceso
          for (i = 0; i < numprocs; i++) {
            //Si ya no hay más elementos que compartir
            if(limit == 1){
              *(scatterSendCounts+i) = 0;
            }
            else{
              //Si es el último proceso, asignarle los elementos restantes
              if(i == numprocs - 1){
                *(scatterSendCounts+i) = remainder;
              } else{
                //Si elementos restantes son menores al número de elementos
                //que se están repartiendo,
                //asignar lo que falta y ya no hay más elementos a repartir
                if(remainder < colPerProc*rows2){
                  *(scatterSendCounts+i) = remainder;
                  limit = 1;
                } else{
                  //Asignar elementos a proceso
                  *(scatterSendCounts+i) = colPerProc * rows2;
                  remainder -= colPerProc * rows2;
                }
              }
            }
          }

        //Llenar arreglo de intervalos a repartir
        disp = 0;
        for (i = 1; i < numprocs; i++) {
            disp += *(scatterSendCounts+i-1);
            *(scatterDisp+i) =  disp;
        }

        //Llenar arreglo de elementos que regresará cada proceso
        for (i = 0; i < numprocs; i++) {
            *(gatherSendCounts+i) = *(scatterSendCounts+i)/cols1*rows1;
        }

        //Llenar arreglo de intervalos a regresar
        disp = 0;
        for (i = 1; i < numprocs; i++) {
            disp += *(gatherSendCounts+i-1);
            *(gatherDisp+i) = disp;
        }
    }

    //Compartir arreglos necesarios para scatter y gather con los procesos
    MPI_Bcast(scatterSendCounts, numprocs, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(scatterDisp, numprocs, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(gatherSendCounts, numprocs, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(gatherDisp, numprocs, MPI_INT, 0, MPI_COMM_WORLD);

    //Arreglo donde se guardan los elementos que se le comparten a cada proceso
    colsForProc = (int *) malloc(sizeof(int) * *(scatterSendCounts+ myid));

    //Repartir columnas de la matriz 2 a los procesos
    MPI_Scatterv(matrix2Transposed, scatterSendCounts,scatterDisp, MPI_INT, colsForProc, *(scatterSendCounts+ myid) , MPI_INT, 0, MPI_COMM_WORLD);

    //Parte de la matriz final que regresa cada proceso
    partialFinalMatrix = (int *) malloc(*(gatherSendCounts+ myid) * sizeof(int));

    //Sección paralela donde se hace la multiplicación de matrices
    #pragma omp parallel default(shared) private(i, nh, tid, myresult)
    {
      nh = omp_get_num_threads();
      tid = omp_get_thread_num();

      //Paralelizar for anidado
      #pragma omp for collapse(2)
        //Ciclo para determinar cual columna de las que le llegaron al proceso está en uso
        for(int colCount = 0; colCount < *(scatterSendCounts + myid) / cols1; colCount++){
          //Ciclo para multiplicar la columna actual con todas las filas de la matriz 1
          for(int rowCount = 0; rowCount < rows1; rowCount++){
            myresult = 0;
            //Ciclo para multiplicar los elementos de la columna por los de la fila y sumarlos
            for(i = 0; i < cols1; i++) {
                myresult += *(matrix1 + rowCount*cols1 + i) * *(colsForProc + colCount*rows2 + i);
                //printf("\t --- Procesador %d (%s). Soy el hilo %d y calculo == %d, %d - %d, %d - %d\n",
                  //myid, hostname, tid, myresult, *(matrix1+rowCount*cols1+i),*(colsForProc+colCount*rows2+i), i, rowCount);
            }

            //Guardar resultado de multiplicación en espacio de la matriz final
            *(partialFinalMatrix + colCount*rows1 + rowCount) = myresult;
            //printf("\t --- Soy el hilo %d y calculo == %d, %d - %d\n",
              //tid, *(partialFinalMatrix+colCount*rows1+rowCount), colCount, rowCount);
          }
        }
    }

    //Juntar todas las porciones de la matriz final calculadas por los procesos en una sola matriz
    MPI_Gatherv(partialFinalMatrix, *(gatherSendCounts+ myid), MPI_INT, gatheredMatrix, gatherSendCounts, gatherDisp, MPI_INT, 0, MPI_COMM_WORLD);

    //Nodo master
    if (myid == 0)
    {

        //Transponer matriz final para que quedé en el orden filas/columnas correcto
        transpose(gatheredMatrix,finalMatrix, cols2, rows1);

        stop = MPI_Wtime();

        printf("Final Matrix\n");
        printMatrix(finalMatrix, rows1, cols2);
        printf("\n");

        printf("Tiempo de ejecución de la sección total paralela = %f \n", stop-startTotal);
        printf("Tiempo de ejecución de la sección de multiplicación paralela = %f \n", stop-startMult);
    }

    //Liberar memoria
    free(matrix1);
    free(matrix2);
    free(gatheredMatrix);
    free(colsForProc);
    free(matrix2Transposed);
    free(finalMatrix);
    free(partialFinalMatrix);
    free(scatterSendCounts);
    free(gatherSendCounts);
    free(scatterDisp);
    free(gatherDisp);

    //Finalizar entorno
    MPI_Finalize();

    return 0;
}
