/*
Programa que recorre un arbol a partir de un directorio y genera un histograma dependiendo el tamaño de los archivos
2 - sept - 2020
Sabrina Santana A01025397
Rubén Sánchez A01021759
*/

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

int maximo = 0;

//Estructura de histograma
typedef struct{
	long int urnaInicio;
	long int urnaFin;
    int noArchivos;
} histograma;

//Funcion que calcula la relacion de * a imprimir dependiendo del maximo numero de archivos
float relacionHistograma(int maximo){
    if(maximo<=50){
        return 1;
    }
    if(maximo>50 && maximo<=100){
        return 0.5;
    }
    if(maximo>100 && maximo<=1000){
        return 0.1;
    }
    if(maximo>1000 && maximo<=10000){
        return 0.01;
    }
    if(maximo>10001){
        return 0.001;
    }
}
//Funcion que da formato al histograma
void printHistograma(histograma * hist, int maximo){
    histograma * final = hist + 9;
    histograma * aux = hist;
    int i;
    float relacion;

    //Calculo del maximo numero de archivos de todas las urnas
    while(aux<final){
        if(aux->noArchivos>maximo){
            maximo = aux->noArchivos;
        }
        aux++;
    }

    aux = hist;

    printf("Urna \t\tNo. archivos \t\t Histograma \n");
    while(aux<final){
        i = 0;
        printf("%ld - %ld \t\t %d\t\t", aux->urnaInicio,aux->urnaFin, aux->noArchivos);
        relacion = relacionHistograma(maximo);
        while(i<(aux->noArchivos)*relacion){
            printf("*");
            i++;
        }
        printf("\n");
        aux++;
    }
}

//Funcion que pasa por cada archivo del arbol aumentando el contador de archivos de la respectiva urna
void busquedaRecursiva(char * nomFile, histograma * hist){
    struct stat fileStatTemp;
    struct dirent * pDirent;
    DIR * dirTemp;
    int tempInt;
    histograma * final = hist + 9;
    histograma * aux = hist;
    
    //Llamada al sistema que abre el directorio
    dirTemp = opendir(nomFile);

        if(dirTemp==NULL){
            return;
        } else {
                while((pDirent=readdir(dirTemp))!=NULL){
                    //Comparacion del nombre del archivo para no contar los directorios "." y ".."
                    if(strcmp(pDirent->d_name,".")==0|strcmp(pDirent->d_name,"..")==0){
                        continue;
                    }
                    
                    //Llamada al sistema que proporciona la informacion del archivo
                    stat(pDirent->d_name, &fileStatTemp);
                
                    //Identificar si el archivo es un directorio
                      if(pDirent->d_type == DT_DIR){
                        busquedaRecursiva(pDirent->d_name, hist); 
                    } else {
                        printf("%s   ",pDirent->d_name); 
                        printf("  Size: \t%ld bytes\n", fileStatTemp.st_size);
                        aux = hist;
                        //Aumentar contador de la urna si el tamaño del archivo corresponde
                        while(aux<final){
                            if(fileStatTemp.st_size>=aux-> urnaInicio && fileStatTemp.st_size<=aux-> urnaFin){
                                int auxInt = aux->noArchivos;
                                 auxInt++;
                                aux->noArchivos = auxInt;
                            } 
                            aux++;
                        }
                    }
                }  
        }
    }

int main(){

    char * nomFile = (char *) malloc(sizeof(char)*25);
    histograma * hist = (histograma *) malloc(sizeof(histograma)*9);

    printf("\nIngrese el path del directorio: ");
    scanf("%s", nomFile);

    //Inicializacion de histograma con urnas
    hist -> urnaInicio = 0;
    hist -> urnaFin = 1023;
    hist -> noArchivos = 0;
    (hist + 1) -> urnaInicio = 1024;
    (hist + 1) -> urnaFin = 2047;
    (hist + 1) -> noArchivos = 0;
    (hist + 2) -> urnaInicio = 2048;
    (hist + 2) -> urnaFin = 3071;
    (hist + 2) -> noArchivos = 0;
    (hist + 3) -> urnaInicio = 3072;
    (hist + 3) -> urnaFin = 4095;
    (hist + 3) -> noArchivos = 0;
    (hist + 4) -> urnaInicio = 4096;
    (hist + 4) -> urnaFin = 8191;
    (hist + 4) -> noArchivos = 0;
    (hist + 5) -> urnaInicio = 8192;
    (hist + 5) -> urnaFin = 16383;
    (hist + 5) -> noArchivos = 0;
    (hist + 6) -> urnaInicio = 16384;
    (hist + 6) -> urnaFin = 32767;
    (hist + 6) -> noArchivos = 0;
    (hist + 7) -> urnaInicio = 32768;
    (hist + 7) -> urnaFin = 65535;
    (hist + 7) -> noArchivos = 0;
    (hist + 8) -> urnaInicio = 65536;
    (hist + 8) -> urnaFin = 131071;
    (hist + 8) -> noArchivos = 0;

    printf("Archivos encontrados: \n");
    busquedaRecursiva(nomFile, hist);
    printf("\nHistograma ...  \n");
    printHistograma(hist,maximo);

    free(nomFile);
    free(hist);

}