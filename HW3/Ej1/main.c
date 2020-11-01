/*
Program that simulates a FIFO GANG scheduler for process assignation of CPU Time
Team:
Sabrina Santana A01025397
Rubén Sánchez A01021759
*/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


//Struct de proceso
typedef struct{
	int id;
	int tiempo;
    int group;
    int running;
} process_t;

//Struct nodo de la cola
typedef struct node{
	process_t * proc;
	struct node *next;
}node_t;


//Variables globales
node_t * cabeza=NULL;
int numProcesos;


//Función para encolar procesos
void encolar(int id, int tiempo, int group){
	node_t *curr=cabeza;

    //Si es el primer proceso de la cola
	if(numProcesos==0){
        //Se crea proceso
        process_t * proceso = (process_t *)malloc(sizeof(process_t));
        curr->proc = proceso;
		curr->proc->id=id;
        curr->proc->tiempo=tiempo;
        curr->proc->group=group;
	}
    //Si no es el primer proceso de la cola
    else{
		//Se llega al final de la cola
		while(curr->next!=NULL){
			curr=curr->next;
		}
		//Se crea proceso
        process_t * proceso = (process_t *) malloc(sizeof(process_t));
        //Se crea nodo
		curr->next=malloc(sizeof(node_t));
        curr->next->proc = proceso;
		curr->next->proc->tiempo=tiempo;
		curr->next->proc->id=id;
        curr->next->proc->group=group;
		curr->next->next=NULL;
	}
	numProcesos++;
}


//Función para desencolar proceso
//Regresa proceso
process_t * desencolar(){
	process_t * returnProcess;
    //Si no hay procesos
    if(numProcesos==0){
        returnProcess=NULL;
    }
    //Si hay un sólo proceso
	else if(numProcesos==1){
		returnProcess = cabeza->proc;
    //Si hay más de un proceso
	}else{
		node_t * sigProc = cabeza->next;
		returnProcess=cabeza->proc;
		free(cabeza);
		cabeza = sigProc;
	}
	numProcesos--;

	return returnProcess;
}

//Main
int main(){
    //Variables
    int numPro, numCPU, opc=-1,contador=0, runTimeResp, idResp, groupResp;
    int quantum=4;
    int * tempCPU;

    cabeza = malloc(sizeof(node_t));
    numProcesos = 0;
    cabeza->next = NULL;

    //Pidiendo valores
	printf("Bienvenido\n\n");
    printf("Escriba el número total de procesos: ");
    scanf("%d", &numPro);
    printf("Escriba el número de CPUs: ");
    scanf("%d", &numCPU);

    //CPUs
    process_t * cpus = (process_t *)malloc(sizeof(process_t)*numCPU);

    //Pidiendo procesos por grupo
    printf("A continuacion ingrese los procesos por grupo: \n");
    while(opc != 0 && numPro>numProcesos){
        printf("1. Ingresar procesos a grupo %d.\t\t0. Continuar\n", contador+1);
        scanf("%d",&opc);
        if(opc > 0){
            contador++;
            opc = 2;
            while(opc == 2 && numPro>numProcesos){
                printf("Grupo: %d \n", contador);
                idResp = numProcesos;
                printf("Escriba el tiempo para el proceso %d: \n ", idResp);
                scanf("%d", &runTimeResp);
                groupResp = contador;
                encolar(idResp,runTimeResp,groupResp);
                printf("2. Ingresar otro proceso en este grupo.\t\t1. Regresar\n");
                scanf("%d",&opc);
            }
        }
    }

    printf("Ya fueron agregados %d procesos.\n\n", numPro);
    printf("Planificación por pandillas con quantum de 4 segundos.\n\n");

    //Planificación mientras la cola tenga procesos
    while(numProcesos>0){
        //Metiendo procesos a los CPU
        for(int i=0; i<numCPU; ++i){
            if(numProcesos>0){
                process_t * desencolard = desencolar();
                printf("El cpu %d ha sido ocupado por el proceso %d del grupo %d\n",
                i,desencolard -> id, desencolard->group);
                printf("Runtime restante: %d\n", desencolard->tiempo);
                desencolard -> tiempo = desencolard -> tiempo - quantum;
                
                (cpus+i)->id=desencolard->id;
                (cpus+i)->tiempo=desencolard->tiempo;
                (cpus+i)->group=desencolard->group;
                (cpus+i)->running=1;

                if(desencolard -> tiempo <=0){
                    free(desencolard);
                }
                
            }
        }
        sleep(4);
        //Regresando procesos a la cola si no han acabado
        for(int i=0; i<numCPU; ++i){
            if((cpus+i) ->running>0){
                (cpus+i) -> running=0;
                if((cpus+i) -> tiempo > 0){
                    //printf("Enter process %d with %d time\n", (cpus+i)->id, (cpus+i)->tiempo);
                    encolar((cpus+i)->id,(cpus+i)->tiempo,(cpus+i)->group);
                }
                else{
                    printf("Proceso %d terminó su ejecución.\n", (cpus+i)->id);
                }
            }
        }
        printf("Fin de quantum\n\n");
        sleep(1);
    } 

    //Liberando memoria
    free(cpus);
    free(cabeza);
}