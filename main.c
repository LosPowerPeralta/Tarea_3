#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include "hashmap.c"
#include "list.c"
#include "treemap.c"

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
    TreeMap* wordFrecuency;
    TreeMap* wordRelevancy;
    unsigned long cantPalabra;
}Libro;

typedef struct{
    TreeMap* Libros;
    unsigned long cantLibros;
}Library;

int lower_than_string(void* key1, void* key2){
    char* k1=(char*) key1;
    char* k2=(char*) key2;
    if(strcmp(k1,k2)<0) return 1;
    return 0;
}

int lower_than_int(void* key1, void* key2){
    int* k1=(int*) key1;
    int* k2=(int*) key2;
    if(*k1 < *k2) return 1;
    return 0;
}

Word* createPalabra(char* str){
    Word* NewWord = (Word*) malloc( sizeof(Word) );
    NewWord->palabra = (char*) malloc( strlen(str)+1);
    strcpy(NewWord->palabra,str);
    NewWord->frecuencia = 1;
    return NewWord;
}

Libro* createLibro(){
    Libro* libroActual = (Libro*) malloc(sizeof(Libro));
    libroActual->cantCaracter = 0;
    libroActual->cantPalabra = 0;
    libroActual->wordFrecuency = createTreeMap(lower_than_int);
    libroActual->wordRelevancy = createTreeMap(lower_than_int);
    return libroActual;
}

Library* createBiblioteca(){
    Library* newLibrary = (Library*) malloc(sizeof(Library));
    newLibrary->Libros = createTreeMap( lower_than_string );
    newLibrary->cantLibros = 0;
    return newLibrary;
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
    while( fscanf(fp, " %1023s", linea)){
        if (strcmp(linea, "Title:") == 0){
            fgetc(fp);
            fgets(titulo,100,fp);
            break;
        }
    }

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
            char folder[11]; strcpy(folder,".\\Libros\\"); //Carpeta donde se ubican los libros
            char archivo[21];
            strcpy(archivo, strcat( folder, (char*)entrada->d_name )); //Ubicación de cada archivo
            procesoArchivo(archivo, titulo);
            char* data = (char*) malloc(21); 
            strcpy(data, archivo);
            char* key = (char*) malloc(101);
            strcpy(key, titulo);
            insertMap(MapLibros, key ,data);
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

void contarCaracteres( char* palabraAux, Libro* libro){
    int cont;
    for(cont = 0 ; palabraAux[cont]!= '\0'; cont += 1 ){
        libro->cantCaracter += 1;
    }
    libro->cantCaracter += 1;
}

void mostrarPalabras(HashMap* MapPalabras){
    Pair* aux = firstMap(MapPalabras);
    Word* palabra;
    while(aux){
        palabra = aux->value;
        printf("%s - %i\n", palabra->palabra, palabra->frecuencia);
        aux = nextMap(MapPalabras);
    }
}

void removerCaracteresEspeciales(char *str) {
  int indiceCadena = 0, indiceCadenaLimpia = 0;
  while (str[indiceCadena]) {
    if ( (str[indiceCadena] >= 65 && str[indiceCadena] <= 90) || (str[indiceCadena] >= 97 && str[indiceCadena] <= 122 )) {
      str[indiceCadenaLimpia] = str[indiceCadena];
      indiceCadenaLimpia++;
    }
    indiceCadena++;
  }
  str[indiceCadenaLimpia] = 0;
}

void ubicarPosicionDeLectura(FILE* fp){
    char linea[1024];
    while(true){
        fscanf(fp, " %1023s", linea);
        if(strcmp(linea,"***") == 0){
            fgets(linea,1023,fp);
            break;
        }
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
    ubicarPosicionDeLectura(fp);
    
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
            contarCaracteres(word, libro);
            removerCaracteresEspeciales(word);
            palabraAux = createPalabra(word);
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

void importar(HashMap* MapLibros, Library* biblioteca) {
    system("cls");
    mostrarTitulos(MapLibros);

    char titulo[101];
    printf("\nIngrese un titulo a la vez (Ingrese 0 para salir): \n");
    do{
        fflush(stdin);
        gets(titulo);
        if(searchMap(MapLibros, titulo)){
            if( searchTreeMap(biblioteca->Libros, titulo) ){
                printf("El libro ya fue agregado a la biblioteca\n");
            }
            else{
                Libro* libroActual = createLibro();
                strcpy(libroActual->titulo, titulo);
                LeerArchivo( (char*)searchMap(MapLibros, titulo)->value , libroActual );
                printf("nombre: %s\n",libroActual->titulo);
                printf("codigo: %s\n",libroActual->codigo);
                insertTreeMap(biblioteca->Libros, libroActual->titulo, libroActual); //INSERTAR  LIBRO EN EL TREEMAP DE LA BIBLIOTECA
                printf("Libro agregado a la biblioteca\n");
            }
    
        } 
        else{
            if( strcmp(titulo, "0") != 0)
            printf("Este titulo no se encuentra en el listado\n");
        }
    }while(strcmp("0", titulo) != 0);
    system("pause");

}

void mostrarLibros(Library* biblioteca){
    system("cls");
    printf("\nLIBROS DE LA BIBLIOTECA\n");
    PairTree* aux = firstTreeMap(biblioteca->Libros);
    while(aux){
        Libro* libroActual = aux->value;

        printf("Codigo del Libro: %s\n", libroActual->codigo);
        printf("Nombre del Libro: %s\n", libroActual->titulo);
        printf("Cantidad de palabras del Libro: %i\n", libroActual->cantPalabra);
        printf("Cantidad de caracteres del Libro ( Sin considerar saltos de linea ): %i\n\n", libroActual->cantCaracter);

        aux = nextTreeMap(biblioteca->Libros);
    }
    system("pause");
}

int main() {
    //system("color 7c");
    Library* biblioteca = createBiblioteca();
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
                importar( MapArchivos, biblioteca );
                break;
            case 2: 
                mostrarLibros( biblioteca );
                break;
            /*case 3: 
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
