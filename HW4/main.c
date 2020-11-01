/*
    Nombre: Rubén Sánchez
    Matrícula: A01021759
    Materia: Programación Avanzada
    Fecha: 10/09/20
    Tarea 4
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>

//Estrctura con valores de procesos
typedef struct{
    int pidT;
    int prom;
} ProcVals;

//Funciones
void print_help();
float promedio(int parent, int child);
void printAst(int asts);

//Main
int main(int argc, char * const * argv) {
    //Variables
    int index, numero=0, estado, relacion;
    int c;
    int help = 0;
    int parent, child;
    float result;
    int max=0;
    int lim;
    
    opterr = 0;
    
    //Ciclo para obtener argumentos
    while ((c = getopt (argc, argv, "n:h")) != -1)
        switch (c)
    {
        //Caso del número de procesos
        case 'n':
            numero = atoi(optarg);
            break;
        //Caso de la ayuda
        case 'h':
            help = 1;
            break;
        //Casos no reconocidos
        case '?':
            if (optopt == 'n')
                fprintf (stderr, "La opción -%c requiere un argumento.\n", optopt);
            else if (isprint (optopt))
                fprintf (stderr, "La opción es desconocida: '-%c'.\n", optopt);
            else
                fprintf (stderr,
                         "La opción es desconocida: '\\x%x'.\n",
                         optopt);
            return 1;
        default:
            abort ();
    }
    
        
    for (index = optind; index < argc; index++)
        printf ("El argumento no es  válido %s\n", argv[index]);


    //Si el número de procesos es válido
    if(numero>0){
        //Arreglo de pids
        pid_t * pids = (pid_t *) malloc(numero * sizeof(pid_t));
        //Arreglo de pids como int y promedios
        ProcVals * tabla = (ProcVals *) malloc(numero * sizeof(ProcVals));
        
        lim=numero;

        printf("Estamos en el proceso padre con PID = %d y su padre es PPID = %d \n", \
                getpid(), getppid());
        //Ciclo para crear los procesos y sacar los promedios
        for (int i=0; i < numero; ++i) {
            *(pids+i) = fork();
        
            if (*(pids+i) == -1)
            {
                sleep(1);
                lim=i;
                printf("Error al crear el proceso hijo. %d procesos fueron creados.\n", lim);
                break;
            }
            else if (*(pids+i)  == 0)
            {
                parent=getppid();
                child=getpid();
                result=promedio(parent, child);
                printf("Soy el proceso %d hijo con PID = %d y mi promedio es = %.2f\n", i, child, result);
                return result;
            }
        }
        sleep(2);
        printf("\n");
        
        //Ciclo para guardar los promedios en el arreglo "tabla" en la estructura padre
        for (int i=0; i < lim; ++i) {
            
            if (waitpid(*(pids+i), &estado, 0) != -1)
                {
                    if (WIFEXITED(estado)){
                        (tabla+i)->pidT=*(pids+i);
                        (tabla+i)->prom=WEXITSTATUS(estado);

                        if(max<(tabla+i)->prom){
                            max=(tabla+i)->prom;
                        }
                    }
                }
        }
        //Cantidad a dividir para saber número de asteriscos
        int relacion=(max/10);

        //Tabla final
        printf("PID Hijo\tPromedio\tHistograma\n");
        for (int i=0; i < lim; ++i) {
            
            printf("%d\t\t%d\t\t", (tabla+i)->pidT, (tabla+i)->prom);
            printAst(((tabla+i)->prom)/relacion);
            printf("\n");
        }

        //Liberando memoria
        free(pids);
        free(tabla);

    }
    else{
        printf("Entrada no válida.\n");
    }
    
    if (help == 1) {
        print_help();
    }
    
    return 0;
    
}

//Función para imprimir ayuda
void print_help()
{
    printf("\nUse: ./a.out [-n value] [-h]\n");
    printf("\nOpciones:\n");
    printf("-n : Entrar número de procesos\n-h : Ayuda\n");
}

//Funcion para sacar el promedio
float promedio(int parent, int child){
    return ((float)parent+(float)child)/2;
}

//Función para escribir asteriscos
void printAst(int asts){
    for(int i=0; i<asts; ++i){
        printf("*");
    }
}