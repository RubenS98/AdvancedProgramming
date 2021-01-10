/*
  Proyecto final programación avanzada
  main.c
  multiplicacion secuencial de matrices

  Equipo 5
  Rubén Sánchez A01021759
  Octavio Garduza A01021660
  Christian Dalma A01423166
  Sabrina Santana A01025397
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include "mpi.h"

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
    double timeTotal, timeMult, stop;
    int * matrix1, * matrix2, * gatheredMatrix, * colsForProc, * matrix2Transposed;
    int * finalMatrix, * partialFinalMatrix;
    int * scatterSendCounts, * gatherSendCounts, * scatterDisp, * gatherDisp;


    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);

    //Pedir dimensiones de las matrices al usuario
      printf("Bienvenido al sistema de planificación urbana.\n");

      printf("Introduzca cuantas filas tiene la matriz 1:\n");
      scanf("%d", &rows1);
      printf("Introduzca cuantas columnas tiene la matriz 1:\n");
      scanf("%d", &cols1);
      printf("Introduzca cuantas filas tiene la matriz 2:\n");
      scanf("%d", &rows2);
      printf("Introduzca cuantas columnas tiene la matriz 2:\n");
      scanf("%d", &cols2);

    //Si no se pueden multiplicar las matrices, terminar programa
    if(cols1 != rows2){
      printf("Las matrices no se pueden multiplicar.\n");
      return 0;
    }

    //Asignar memoria
    matrix1 = (int *) malloc(rows1 * cols1 * sizeof(int));
    matrix2 = (int *) malloc(rows2 * cols2 * sizeof(int));
    finalMatrix = (int *) malloc(rows1 * cols2 * sizeof(int));
    gatheredMatrix = (int *) malloc(cols2*rows1* sizeof(int));
    matrix2Transposed = (int *) malloc(cols2 * rows2 * sizeof(int));

    //timeTotal = clock();
    timeTotal = MPI_Wtime();

    srand(seed);

    //Crear matriz 1 con datos aleatorios
    for(i = 0; i < rows1*cols1; i++) {
      *(matrix1 + i) = rand() % 500 + 100;
    }

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
    timeMult = MPI_Wtime();

    //Transponer matriz 2
    transpose(matrix2, matrix2Transposed, rows2, cols2);

    printf("\n");

    for(int colCount = 0; colCount < cols2; colCount++){
      for(int rowCount = 0; rowCount < rows1; rowCount++){
        myresult = 0;
        //Ciclo para multiplicar los elementos de la columna por los de la fila y sumarlos
        for(i = 0; i < cols1; i++) {
            myresult += *(matrix1 + rowCount*cols1 + i) * *(matrix2Transposed + colCount*rows2 + i);
          }
        //Guardar resultado de multiplicación en espacio de la matriz final
        *(gatheredMatrix + colCount*rows1 + rowCount) = myresult;
      }
    }


  //Transponer matriz final para que quedé en el orden filas/columnas correcto
  transpose(gatheredMatrix,finalMatrix, cols2, rows1);
/*
  printf("Final Matrix\n");
  printMatrix(finalMatrix, rows1, cols2);
  printf("\n");
*/
  stop = MPI_Wtime();

  printf("Tiempo de ejecución de la sección total paralela = %f \n", stop-timeTotal);
  printf("Tiempo de ejecución de la sección de multiplicación paralela = %f \n", stop-timeMult);

    //Liberar memoria
    free(matrix1);
    free(matrix2);
    free(gatheredMatrix);
    free(matrix2Transposed);
    free(finalMatrix);

    return 0;
}
