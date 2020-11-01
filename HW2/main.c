/*
    Nombre: Rubén Sánchez
    Matrícula: A01021759
    Materia: Programación Avanzada
    Fecha: 27/08/20
    Tarea 2
*/
#include <stdio.h>
#include <stdlib.h>

#define N 10

//Tipo de dato iterador
typedef struct {
    void * start;
    void * last;
    void * current;
    size_t size;

    void * (*begin)(struct iterador * self);
    void * (*end)(struct iterador * self);
    void * (*prev)(struct iterador * self);
    void * (*next)(struct iterador * self);
} iterador;

//Tipo de dato libro
typedef struct{
    char * titulo;
    int paginas;
} Libro;

//Tipo de función

typedef void (*t_imp)(void *);
typedef void (*t_iterador)(iterador *, t_imp);

//Iteradores

void recorre(t_iterador, void *, size_t, size_t, t_imp);

void forIt(iterador * it, t_imp imp);
void revIt(iterador * it, t_imp imp);
void bidIt(iterador * it, t_imp imp);

//Constructor
iterador * constructor(void *, size_t, size_t);

//Funciones del iterador
void * begin(iterador *);
void * end(iterador *);
void * prev(iterador *);
void * next(iterador *);

//Funciones de impresión

void imprimeInt(void *);
void imprimeFloat(void *);
void imprimeDouble(void *);
void imprimeLibro(void *);
void imprimeChar(void *);

//Main
int main(int argc, const char * argv[])
{
    int tam, opc;
    printf("Bienvenido\n\n");
    //Int
    printf("Colección de enteros\n\n");
    printf("Escriba el número de elementos que tendrá el arreglo de enteros:\n");
    scanf("%d", &tam);

    if(tam>0){
        int * vector = (int *) malloc(tam * sizeof(int));
    
        int * aux = vector;
        int * last = vector + tam;

        for (; aux < last; ++aux) {
            *aux = rand() % 100;
        }

        printf("Cuál iterador quiere usar?(Forward=1, Reverse=2, Bi=3)\n");
        scanf("%d", &opc);
        printf("\n");
        if(opc==1){
            recorre(&forIt, vector, sizeof(*vector), tam, &imprimeInt);
        }
        else if(opc==2){
            recorre(&revIt, vector, sizeof(*vector), tam, &imprimeInt);
        }
        else{
            recorre(&bidIt, vector, sizeof(*vector), tam, &imprimeInt);
        }
        
        free(vector);
    }

    printf("\n");

    //Libro
    printf("Colección de libros\n\n");
    
    printf("Entre el número de libros: ");
    scanf("%d", &tam);

    if(tam>0){
        Libro * biblio;
        biblio = (Libro *) malloc(tam * sizeof(Libro));
        
        Libro * auxL = biblio;
        Libro * lastL = biblio + tam;

        
        for (int i=1; auxL < lastL; ++auxL) {
            auxL->titulo = (char * ) malloc(sizeof(char) * 30);
            printf("Entre el nombre del libro %d: ", i);
            scanf(" %[^\n]", auxL->titulo);

            printf("Entre el número de páginas: ");
            scanf("%d", &auxL->paginas);
            ++i;
        }
        printf("\n");

        printf("Cuál iterador quiere usar?(Forward=1, Reverse=2, Bi=3)\n");
        scanf("%d", &opc);
        printf("\n");

        if(opc==1){
            recorre(&forIt, biblio, sizeof(Libro), tam, &imprimeLibro);
        }
        else if(opc==2){
            recorre(&revIt, biblio, sizeof(Libro), tam, &imprimeLibro);
        }
        else{
            recorre(&bidIt, biblio, sizeof(Libro), tam, &imprimeLibro);
        }

        for(auxL=biblio; auxL < lastL; ++auxL){
            free(auxL->titulo);
        }

        free(biblio);
    }
    
    return 0;
}

//Constructor del iterador
//Recibe un arreglo, el tamaño de sus elementos y su tamaño
iterador * constructor(void * vector, size_t size, size_t count){
    iterador * it = malloc(sizeof(iterador));

    it->start=vector;
    it->last=vector+(count*size)-size;
    it->current=vector;
    it->size=size;
    
    it->begin = &begin;
    it->end = &end;
    it->next = &next;
    it->prev = &prev;

    return it;
}

//Función genérica

void recorre(t_iterador itera, void * vector, size_t size, size_t count, t_imp imp){
    iterador * it = constructor(vector, size, count);
    (*itera)(it, imp);
    free(it);
}

//Iteradores

void forIt(iterador * it, t_imp imp){
    it->current=it->begin(it);

    while(it->current!=NULL){
        (*imp)(it->current);
        it->current=it->next(it);
    }
    printf("\n");
}

void revIt(iterador * it, t_imp imp){
    it->current=it->end(it);

    while(it->current!=NULL){
        (*imp)(it->current);
        it->current=it->prev(it);
    }
    printf("\n");
}

void bidIt(iterador * it, t_imp imp){
    forIt(it, imp);
    revIt(it, imp);
    printf("\n");
}

//Funciones del iterador

void * begin(iterador * it){

    return it->start;
}

void * end(iterador * it){

    return it->last;
}

void * next(iterador * it){
    void * temp = it->current + it->size;
    if(temp>it->last){
        return NULL;
    }
    return temp;
}

void * prev(iterador * it){
    void * temp = it->current - it->size;
    if(temp<it->start){
        return NULL;
    }
    return temp;
}

//Funciones para imprimir

void imprimeInt(void * temp){
    printf(" %d ", *((int*)(temp)));
}

void imprimeFloat(void * temp){
    printf(" %f ", *((float*)(temp)));
}

void imprimeDouble(void * temp){
    printf(" %f ", *((double*)(temp)));
}

void imprimeLibro(void * temp){
    printf(" %s, %d ", ((Libro *)(temp))->titulo, ((Libro *)(temp))->paginas);
}

void imprimeChar(void * temp){
    printf(" %c ", *((char*)(temp)));
}