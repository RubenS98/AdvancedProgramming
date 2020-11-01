/*
Conjuntos de señales
Team:
Sabrina Santana A01025397
Rubén Sánchez A01021759
Fecha de entrega: 09/10/20

Programa que genera archivos y guarda en ellos señales pendientes. Usa descriptores de
archivos, grupos de señales y bloqueos.
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

//Estrucutra con nombres de archivos y sus tamaños
typedef struct{
    char nombre[1000];
    unsigned long int tam;
}archivos;

//Variables globales
int grabar=1;
char* dirname = "datos";
char* dirnamePath = "datos/";

//Funciones
void print_help();
void gestorAlarm();
void funcDirectorio();
void itoa(int, char*);
void reverse(char*);

//Main
int main(int argc, char * const * argv) {
    //Variables
    int index, numeroA=0, segundos=0;
    int c;
    int check, fd;
    int help = 0;
    char* x = "x";
    char* ctrlc="2";
    char* ctrlz="20";
    char* espacio=" ";
    char nombresArchivos[1000];
    char nombreArchivo[1000];
    char tempNum[1000];
    int largo;
    
    //Arreglo con señales
    char nombresSig[65][20] = {"", "SIGHUP", "SIGINT","SIGQUIT", "SIGILL", "SIGTRAP", "SIGABRT", "SIGBUS",
    "SIGFPE", "SIGKILL", "SIGUSR1", "SIGSEGV", "SIGUSR2", "SIGPIPE", "SIGALRM", "SIGTERM", 
    "SIGSTKFLT", "SIGCHLD", "SIGCONT", "SIGSTOP", "SIGTSTP", "SIGTTIN", "SIGTTOU", "SIGURG", "SIGXCPU",
    "SIGXFSZ", "SIGVTALRM",	"SIGPROF", "SIGWINCH", "SIGIO", "SIGPWR", "SIGSYS", "SIGRTMIN", "SIGRTMIN+1", "SIGRTMIN+2", "SIGRTMIN+3",
    "SIGRTMIN+4", "SIGRTMIN+5", "SIGRTMIN+6", "SIGRTMIN+7",	"SIGRTMIN+8", "SIGRTMIN+9",	"SIGRTMIN+10", "SIGRTMIN+11", "SIGRTMIN+12",
    "SIGRTMIN+13", "SIGRTMIN+14", "SIGRTMIN+15", "SIGRTMAX-14", "SIGRTMAX-13", "SIGRTMAX-12", "SIGRTMAX-11", "SIGRTMAX-10", "SIGRTMAX-9",
    "SIGRTMAX-8", "SIGRTMAX-7", "SIGRTMAX-6", "SIGRTMAX-5", "SIGRTMAX-4", "SIGRTMAX-3", "SIGRTMAX-2", "SIGRTMAX-1", "SIGRTMAX"};
    
    sigset_t todas, pendientes;

    struct sigaction gestorAnterior;
    struct stat fileStatTemp;

    opterr = 0;
    
    //Ciclo para obtener argumentos
    while ((c = getopt (argc, argv, "n:t:h")) != -1)
        switch (c)
    {
        //Caso del número de archivos
        case 'n':
            numeroA = atoi(optarg);
            break;
        //Caso de los segundos del temporizador
        case 't':
            segundos = atoi(optarg);
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

    printf("Bienvenido a la actividad 7\n");
    printf("\n");

    //Validar que argumentos son válidos
    if(numeroA>0 && segundos>0){
        //Arreglo de archivos
        archivos* files = (archivos*)malloc(sizeof(archivos)*numeroA);

        //Agregar todas las señales a set
        sigfillset(&todas);
        //Quitar SIGALRM de set
        sigdelset(&todas, SIGALRM);
        //Bloquear todas las señales del set (SIGALRM no incluida)
        sigprocmask(SIG_BLOCK, &todas, NULL);
        //Guardar gestor inicial de SIGALRM
        sigaction(SIGALRM, NULL, &gestorAnterior);
        //Establecer gestor de SIGLALRM
        signal(SIGALRM, gestorAlarm);

        //Llamar a función que crea/limpia directorio
        funcDirectorio();
        printf("\n");

        //Ciclo para crear archivos
        for(int i=0; i<numeroA; ++i){
            //Crear nombre de archivo con path
            strcpy(nombresArchivos, dirnamePath);
            strcat(nombresArchivos, "a");
            itoa(i, tempNum);
            strcat(nombresArchivos, tempNum);
            strcat(nombresArchivos, ".txt");
            
            //Crear nombre de archivo sin path
            strcpy(nombreArchivo, "a");
            strcat(nombreArchivo, tempNum);
            strcat(nombreArchivo, ".txt");

            //Abrir archivo
            fd = open(nombresArchivos, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU, S_IRWXG, S_IRWXO);

            //Establecer espera
            alarm(segundos);

            //Ciclo para escribir 'x' en archivo
            while(grabar){
                write(fd, x, sizeof(char));
            }

            //Restaurar variable grabar
            grabar=1;

            //Obtener señales pendientes
            sigpending(&pendientes);

            //Ciclo para buscar señales en el set de pendientes
            //y escribirlas en el archivo
            for(int j=1; j<64; j++){
                if(sigismember(&pendientes, j)){
                    write(fd, espacio, sizeof(char));
                    printf("Señal %s pendiente\n", nombresSig[j]);
                    largo=strlen(nombresSig[j]);
                    write(fd, nombresSig[j], sizeof(char)*largo);
                }
            }
            
            //Cerrar archivo
            close(fd);

            //Imprimir nombre y tamaño de archivo
            stat(nombresArchivos, &fileStatTemp);
            printf("Archivo creado: %s\n", nombreArchivo);
            printf("Size: %ld bytes\n", fileStatTemp.st_size);

            //Guardar nombre y tamaño en arreglo de archivos
            strcpy((files+i)->nombre, nombreArchivo);
            (files+i)->tam=fileStatTemp.st_size;

            printf("\n");

        }

        printf("Imprimiendo archivos y sus tamaños\n");
        printf("\n");
        //Ciclo para imprimir arreglo de archivos
        for(int i=0; i<numeroA; i++){
            printf("File: %s\t\tSize: %ld bytes\n", (files+i)->nombre, (files+i)->tam);
        }

        //Liberar memoria
        free(files);

        //Restablecer manejador
        sigaction(SIGALRM, &gestorAnterior, NULL);
    }
    //Caso no válido
    else{
        printf("Entrada no válida. Se le deben de dar argumentos a 'n' y a 't' que sean mayores a 0.\n");
    }

    //Imprimir ayuda
    if (help == 1) {
        print_help();
    }
    
    return 0;
    
}

//Gestor de SIGALRM, pone variable grabar en 0
void gestorAlarm(int sid)
{
    grabar=0;
}

//Función para imprimir ayuda
void print_help()
{
    printf("\nUse: ./a.out [-n value] [-t value] [-h]\n");
    printf("\nOpciones:\n");
    printf("-n : Entrar número de archivos\n");
    printf("-t : Entrar segundos del temporizador\n-h : Ayuda\n");
}

//Función para crear directorio "datos" si no existe o
//para borrar archivos de directorio "datos"
void funcDirectorio(){
    struct dirent * pDirent;
    DIR * dirTemp;
    int check;
    char nombreCompleto[1000];
    
    //Llamada al sistema que abre el directorio
    dirTemp = opendir(dirname);

        //Si directorio no existe
        if(dirTemp==NULL){
            //Crear directorio
            check=mkdir(dirname, 0777);
            if(!check){
                printf("Directorio creado\n");
            }
            return;
        //Si directorio existe
        } else {
                printf("Borrando archivos...\n");
                //Por cada archivo en el directorio
                while((pDirent=readdir(dirTemp))!=NULL){
                    //Comparacion del nombre del archivo para no contar los directorios "." y ".."
                    if(strcmp(pDirent->d_name,".")==0|strcmp(pDirent->d_name,"..")==0){
                        continue;
                    }
                    
                    //Imprimir nombre archivo
                    printf("%s\n", pDirent->d_name);
                    strcpy(nombreCompleto, dirnamePath);
                    strcat(nombreCompleto, pDirent->d_name);
                    //Borrar archivo
                    if(remove(nombreCompleto)!=0){
                        printf("File deletion error\n");
                    }
                    
                }  
        }
        printf("\n");
}

//Función para pasar ints a string
void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

//Función para invertir strings
void reverse(char s[])
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}