/*
    Nombre: Rubén Sánchez
    Matrícula: A01021759
    Materia: Programación Avanzada
    Fecha: 20/08/20
    Tarea 1
*/
#include <stdio.h>
#include <stdlib.h>

//Tipo de dato paciente
typedef struct {
    char * nombre;
    char * apellidos;
    int edad;
    char * telefono;
    int cama;
} paciente;

//Funciones
void agregarPaciente(paciente *, paciente *);
void checarCama(paciente *, int, int);
void darDeAlta(paciente *, int, int);
void listado(paciente *, paciente *);
int camasDisp(paciente *, paciente *);

//Main
int main(int argc, const char * argv[]){
    //Variables
    int camas = 0, i, resp, opc, disponibles;
    paciente * hospital;
    paciente * temp;

    printf("Bienvenido\n\n");
    
    printf("¿Cuántas camas tiene el hospital? : ");
    scanf("%d", &camas);
    
    //Asignando memoria a hospital
    hospital = (paciente *) malloc(sizeof(paciente) * camas);
    
    paciente * fin = hospital + camas;
    
    //Creando pacientes
    for (i=0, temp = hospital; temp < fin; ++temp) {
        printf("¿Hay un paciente en la cama %d?(Si=1, No=0): ", i+1);
        scanf("%d", &resp);

        if(resp){
            temp->nombre = (char * ) malloc(sizeof(char) * 20);
            printf("Entre el nombre del paciente: ");
            scanf(" %[^\n]", temp->nombre);

            temp->apellidos = (char * ) malloc(sizeof(char) * 25);
            printf("Entre los apellidos del paciente: ");
            scanf(" %[^\n]", temp->apellidos);

            printf("Entre la edad del paciente: ");
            scanf("%d", &temp->edad);

            temp->telefono = (char * ) malloc(sizeof(char) * 20);
            printf("Entre el telefono del paciente: ");
            scanf(" %[^\n]", temp->telefono);

            temp->cama=i+1;
        }
        else{
            temp->cama=-1;
        }
        ++i;
        
    }
    printf("\n");
    //Menu
    do {
        printf("--- MENU ---\n");
        printf("1-Incorporar paciente\n2-Checar estatus de una cama\n3-Dar de alta a paciente \
        \n4-Mostrar listado de pacientes\n5-Conocer número de camas disponibles\n0-Salir\nSeleccione una opción(número): ");
        scanf("%d",&opc);
        
        switch(opc) {
            
            case 0: break;
            //Incorporar paciente
            case 1:
                printf("Incorporando paciente.\n");

                disponibles=camasDisp(hospital, fin);

                //Si no hay camas disponibles, se le asigna más
                //espacio al hospital
                if (disponibles==0){
                    printf("Noy hay camas disponibles.\n");
                    printf("Incorporando 5 camas más.\n");
                    hospital = (paciente *) realloc(hospital, sizeof(paciente) * (camas + 5));
                    fin=hospital+camas+5;

                    for (temp = hospital+camas; temp < fin; ++temp) {
                        temp->cama=-1;  
                        
                    }
                    camas=camas+5;
                }

                agregarPaciente(hospital, fin);
                break;
            //Checar cama
            case 2:
                printf("Entre el número de la cama que quiere revisar: ");
                scanf("%d", &resp);
                checarCama(hospital, camas, resp);
                break;
            //Dar de alta a paciente
            case 3:
                listado(hospital, fin);
                printf("Entre el número de la cama del paciente a dar de alta: ");
                scanf("%d", &resp);
                darDeAlta(hospital, camas, resp);

                break;
            //Imprimir listado de pacientes
            case 4:
                listado(hospital, fin);
                break;
            //Imprimir número de camas disponibles
            case 5:
                disponibles=camasDisp(hospital, fin);

                printf("Hay %d cama(s) disponible(s) y %d ocupada(s).\n", 
                disponibles, camas-disponibles);
                printf("\n");
                break;
            //Default
            default:
                printf("Opción no válida\n");
        }
        printf("\n");
        
    } while (opc != 0);
    
    //Liberar memoria
    for (temp = hospital; temp < fin; ++temp) {
        free(temp->nombre);
        free(temp->apellidos);
        free(temp->telefono);
    }
    
    free(hospital);
    
    return 0;
}

//Función que agrega paciente a la primera cama disponible
void agregarPaciente(paciente * hospital, paciente * fin){
    paciente * temp;
    int i=1;

    temp=hospital;

    while(temp<fin){
        if((*temp).cama==-1){
            printf("El número de la cama asignada es %d.\n", i);
            temp->nombre = (char * ) malloc(sizeof(char) * 20);
            printf("Entre el nombre del paciente: ");
            scanf(" %[^\n]", temp->nombre);

            temp->apellidos = (char * ) malloc(sizeof(char) * 25);
            printf("Entre los apellidos del paciente: ");
            scanf(" %[^\n]", temp->apellidos);

            printf("Entre la edad del paciente: ");
            scanf("%d", &temp->edad);

            temp->telefono = (char * ) malloc(sizeof(char) * 20);
            printf("Entre el telefono del paciente: ");
            scanf(" %[^\n]", temp->telefono);

            temp->cama=i;
            break;
        }
        ++temp;
        ++i;
    }
    printf("\n");
}

//Función que imprime qué paciente está en una cama
//o si la cama está disponible
void checarCama(paciente * hospital, int camas, int resp){
    paciente * temp;
    
    temp=hospital+resp-1;

    if(resp<=camas && resp>(0)){
        if(temp->cama!=(-1)){
            printf("La cama está siendo ocupada por:\n");
            printf("Nombre: %s\nApellidos: %s\nEdad: %d\nTelefono: %s\n", 
            temp->nombre, temp->apellidos, temp->edad, temp->telefono);
        }
        else{
            printf("Esa cama está disponible.\n");
        }
    }
    else{
        printf("Esa cama no existe.\n");
    }
    printf("\n");
}

//Función que elimina a paciente y deja a cama como
//disponible
void darDeAlta(paciente * hospital, int camas, int resp){
    paciente * temp;
    
    temp=hospital+resp-1;

    if(resp<=camas && resp>(0)){
        if(temp->cama!=(-1)){
            printf("Se dió de alta al paciente:\n");
            printf("Nombre: %s\nApellidos: %s\nEdad: %d\nTelefono: %s\n", 
            temp->nombre, temp->apellidos, temp->edad, temp->telefono);

            temp->cama=-1;

            printf("Cama %d está disponible.", resp);
        }
        else{
            printf("Esa cama está disponible.\n");
        }
    }
    else{
        printf("Esa cama no existe.\n");
    }
    printf("\n");
}

//Función que imprime a los pacientes
void listado(paciente * hospital, paciente * fin){
    paciente * temp;

    printf("\n\n--- Listado de pacientes (Cama, Nombre, Apellidos)---\n\n");
    for (temp = hospital; temp < fin; ++temp) {
        if((*temp).cama!=-1){
            printf("%2d \t %20s \t %25s \n",
                temp->cama,
                temp->nombre,
                temp->apellidos);
        }
    }
    printf("\n");
}

//Función que regresa el número de camas
//disponibles
int camasDisp(paciente * hospital, paciente * fin){
    paciente * temp;
    int disp=0;

    for (temp = hospital; temp < fin; ++temp) {
        if((*temp).cama==-1){
            ++disp;
        }
    }
    return disp;
}