/*
    Nombre: Rubén Sánchez
    Matrícula: A01021759
    Materia: Programación Avanzada
    Fecha: 17/09/20
    Tarea 5
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>

//Struct para almacenar pipes
typedef struct{
    int id;
    int * tuberia;
} Tuberias;

//Funciones
void print_help();
void leer(int * fd, int * fd2);

//Main
int main(int argc, char * const * argv) {
    //Variables
    int index, numero=0, estado;
    int c;
    int help = 0;
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
    
    //Para argumentos no válidos
    for (index = optind; index < argc; index++)
        printf ("El argumento no es válido %s\n", argv[index]);

    //Si argumento es entero mayor a 0
    if(numero>0){
        //Memoria para tuberias y memoria para pids de procesos hijos
        Tuberias * pipes = (Tuberias *) malloc(numero * sizeof(Tuberias));
        pid_t * pids = (pid_t *) malloc(numero * sizeof(pid_t));

        //Ciclo para crear pipes
        for(int i=0; i<numero; ++i){
            (pipes+i)->id=i;
            (pipes+i)->tuberia=(int *) malloc(2 * sizeof(int));
            pipe((pipes+i)->tuberia);
        }

        //Ciclo para crear procesos hijos, asignarles sus tuberias
        //y ponerlos a leer
        for(int i=0; i<(numero-1); ++i){
            *(pids+i) = fork();

            //Proceso hijo
            if (*(pids+i) == 0)
            {
                printf("Estoy leyendo y soy el hijo ... \n");
                leer((pipes+i)->tuberia, (pipes+i+1)->tuberia);
                return 1;
            }
            //Error
            else if (*(pids+i) == -1)
            {
                exit(-1);
            }
        }

        //Crear úlitmo proceso, asignarle pipes
        //y ponerlo a leer

        *(pids+(numero-1)) = fork();
        
        //Proceso hijo
        if (*(pids+(numero-1)) == 0)
        {
            printf("Estoy leyendo y soy el hijo ... \n");
            leer((pipes+(numero-1))->tuberia, (pipes)->tuberia);
            return 1;
        }
        //Error
        else if (*(pids+(numero-1)) == -1)
        {
            exit(-1);
        }

        sleep(2);
        printf("\n");

        //Crear testigo
        char testigo='T';

        //Escribir testigo en el primer pipe
        //desde el proceso padre
        close (*((pipes)->tuberia));
        printf("+++ Primer envío %c \n", testigo);
        printf("\n");
        write(*((pipes)->tuberia+1), &testigo, sizeof(char));

        //Esperar a que procesos hijos acaben
        for (int i=0; i < numero; ++i) {
                
            if (waitpid(*(pids+i), &estado, 0) != -1)
                {
                    //printf("Terminado\n");
                }
        }

        //Liberando memoria
        for (int i=0; i < numero; ++i) {
            free((pipes+i)->tuberia);
        }

        free(pipes);
        free(pids);
    }
    else{
        printf("Entrada no válida\n");
    }

    //Imprimir ayuda
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

//Funcion para leer testigo y escribir testigo en la siguiente tuberia
void leer(int * fd, int * fd2)
{
    int c=2;
    char test;
    
    while(c)
    {
        close(*(fd+1));
        //Leer primer pipe (pipe para leer)
        read(*(fd), &test, sizeof(char));
        printf("—-> Soy el proceso con PID %d y recibí el testigo %c, el cual tendré por 5 segundos\n",
         getpid(), test);
        
        //Esperar
        sleep(5);
        
        //Escribir en siguiente pipe (pipe para escribir)
        printf("<—- Soy el proceso con PID %d y acabo de enviar el testigo %c\n", getpid(), test);
        close (*(fd2));
        write(*(fd2+1), &test, sizeof(char));
    }
    
}