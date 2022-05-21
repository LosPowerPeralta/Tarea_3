#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include "hashmap.c"
#include "list.c"
//#include "treemap.c"

typedef struct{
    char* palabra;
    unsigned long frecuencia;
    unsigned long relevancia;
}Word;

typedef struct{
    char titulo[101];
    char codigo[11];
    unsigned long cantCaracter;
    HashMap* wordSearch;
    //TreeMap* wordFrecuency;
    //TreeMap* wordRelevancy;
    unsigned long cantPalabra;
}Libro;

typedef struct{
    //TreeMap* Libros;
}Biblioteca;

Word* createPalabra(char* str){
    Word* NewWord = (Word*) malloc( sizeof(Word) );
    NewWord->palabra = (char*) malloc( strlen(str)+1);
    strcpy(NewWord->palabra,str);
    NewWord->frecuencia = 1;
    return NewWord;
}

void procesoArchivo(char archivo[16], char* titulo){
    FILE* fp = NULL;
    fp = fopen ( archivo , "r");//Abrir file

    if(!fp){ //Validar que los archivos se abren correctamente en modo lectura
        printf("Error al abrir el archivo");
        system("pause");
        exit (1); 
    }

    char linea[1024];
    //Hay 3 casos distintos para obtener el titulo del libro
    fgets(linea,35,fp);
    fgets(titulo,100,fp);


    titulo[strlen(titulo) - 1] = '\0'; //Elimina el salto de linea

    fclose(fp);//Cerrar file
}

bool esNumero(char *caracter) {
    int cont;
    if( !strcmp(caracter, "0") ) return false;
    for (cont = 0; cont < strlen(caracter); cont++) {
        if ( !isdigit(caracter[cont])  ) {
            return false;
        }
    }

    return true;
}

HashMap* listarArchivos(){

    HashMap *MapLibros = createMap(300);
    DIR *directorio;
    struct dirent *entrada;
    

    directorio = opendir (".\\Libros");
    if (directorio == NULL){
        printf("Error al abrir el directorio");
        system("pause");
        exit (1);
    }
    while ((entrada = readdir (directorio)) != NULL){
        if ( (strcmp(entrada->d_name, ".")!=0) && (strcmp(entrada->d_name, "..")!=0) ){
            char titulo[101];
            //char* titulo = (char*) malloc(101);
            char folder[11]; strcpy(folder,".\\Libros\\"); //Carpeta donde se ubican los libros
            char archivo[21];
            //char* archivo = (char*) malloc(21);
            strcpy(archivo, strcat( folder, (char*)entrada->d_name )); //Ubicación de cada archivo
            procesoArchivo(archivo, titulo);
            char* data = (char*) malloc(21); 
            strcpy(data, archivo);
            char* key = (char*) malloc(101);
            strcpy(key, titulo);
            insertMap(MapLibros, key ,data);
            //insertMap(MapLibros, titulo, archivo);
        }
    }
    closedir (directorio);
    return MapLibros;
}

void quitarFolder(char* codigo , char *ubicacion){
    int i = 0, j = 0;
    while(ubicacion[i] != '\0'){
        if( i >= 9 && i <= strlen(ubicacion)-5){
            codigo[j] = ubicacion[i];
            j += 1 ;
        }
        i += 1;
    }
    codigo[j] = '\0';
}

char* next_word (FILE *f) {
    char x[1024];
    if (fscanf(f, " %1023s", x) == 1)
        return strdup(x);
    else
        return NULL;
}

void contarCaracteres( Word* palabraAux, Libro* libro){
    int cont;
    for(cont = 0 ; palabraAux->palabra[cont]!= '\0'; cont += 1 ){
        libro->cantCaracter += 1;
    }
    libro->cantCaracter += 1;
}

void mostrarPalabras(HashMap* MapPalabras){
    Pair* aux = firstMap(MapPalabras);
    Word* palabra;
    while(aux){
        palabra = aux->value;
        printf("%s\n", palabra->palabra);
        aux = nextMap(MapPalabras);
    }
}

void LeerArchivo(char* ubicacion, Libro* libro){
    FILE* fp = NULL;
    fp = fopen ( ubicacion , "r");//Abrir file

    if(!fp){ //Validar que los archivos se abren correctamente en modo lectura
        printf("Error al abrir el archivo");
        system("pause");
        exit (1); 
    }
    Word* palabraAux;
    libro->wordSearch = createMap(250);
    //Aqui obtengo las palabras de cada archivo y las guardo
    char* word=next_word(fp);
    Pair* aux;
    while(word){
        aux = searchMap(libro->wordSearch, word);
        if( aux ){
           palabraAux = aux->value;
           palabraAux->frecuencia += 1; 
        }
        else{
            palabraAux = createPalabra(word);
            contarCaracteres(palabraAux, libro);
            //SACAR CARACTERES ESPECIALES
            insertMap(libro->wordSearch, palabraAux->palabra, palabraAux );
            libro->cantPalabra += 1;
        }

     word=next_word(fp);
    }
    mostrarPalabras(libro->wordSearch);
    printf("%i - %i", libro->cantPalabra, libro->cantCaracter );
    fclose(fp);
    char codigo[11];
    quitarFolder(codigo, ubicacion);
    strcpy(libro->codigo, codigo );
}

void mostrarTitulos(HashMap* MapLibros){
    Pair* aux = firstMap(MapLibros);
    while(aux){
        printf("%s\n", aux->key);
        aux = nextMap(MapLibros);
    }
}

void importar(HashMap* MapLibros) {
    system("cls");
    mostrarTitulos(MapLibros);

    char titulo[101];
    printf("\nIngrese un titulo a la vez (Ingrese 0 para salir): \n");
    do{
        fflush(stdin);
        gets(titulo);
        if(searchMap(MapLibros, titulo)){
            Libro* libroActual = (Libro*) malloc(sizeof(Libro));
            libroActual->cantCaracter = 0;
            libroActual->cantPalabra = 0;
            strcpy(libroActual->titulo, titulo);
            LeerArchivo( (char*)searchMap(MapLibros, titulo)->value , libroActual );
            printf("nombre: %s\n",libroActual->titulo);
            printf("codigo: %s\n",libroActual->codigo);
            //INSERTAR EN EL TREEMAP DE LA BIBLIOTECA
            printf("Libro agregado a la biblioteca\n");
        } 
        else{
            printf("Este titulo no se encuentra\n");
        }
        

    }while(strcmp("0", titulo) != 0);
    system("pause");

}

int main() {
    //system("color 7c");

    HashMap* MapArchivos = (HashMap*) listarArchivos(); 
    char opcion[2];
    int auxOpcion;

    while(true) {
        system("cls");
        printf("\n========== MENU DE BIBLIOTECA ==========\n\n");
        printf("1.-  Cargar libros\n");
        printf("2.-  Mostrar libros ordenados\n");
        printf("3.-  Buscar un libro por titulo\n");
        printf("4.-  Palabras con mayor frecuencia\n");  
        printf("5.-  Palabras mas relevantes\n"); 
        printf("6.-  Buscar por palabra\n");  
        printf("7.-  Mostrar palabra en su contexto dentro del libro\n");
        printf("8.-  Salir\n");

        printf("\nINGRESE SU OPCION: ");

        while (true) {
            fflush(stdin);
            gets(opcion);
            if (esNumero(opcion)) break;
            else printf("Opcion ingresada incorrecta, intentelo de nuevo: ");
        }

        auxOpcion = atoi(opcion);
        fflush(stdin);

        switch(auxOpcion)
        {
            case 1: 
                importar( MapArchivos );
                break;
            /*case 2: 
                mostrarLibros( almacen->nombre );
                break;
            case 3: 
                buscarTitulo( almacen );
                break;
            case 4: 
                palabrasFrecuentes( almacen->tipo );
                break;
            case 5: 
                palabrasRelevantes( almacen->marca );
                break;
            case 6: 
                buscarXPalabra( almacen->nombre );
                break;
            case 7: 
                mostrarEnContexto( almacen, 0);
                break;*/
            case 8:
                return EXIT_SUCCESS;
        }
    }
    

    return EXIT_SUCCESS;
}
