#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <math.h>
#include "hashmap.c"
#include "list.c"
#include "treemap.c"

typedef struct{
    char* palabra;
    double frecuencia;
    double relevancia;
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
    HashMap* MapCodigo;
    TreeMap* Libros;
}Library;

char* AMinuscula( char* str){
    size_t index = 0;
    size_t largo = strlen( str );
    while ( index < largo)
    {
        str[index] = tolower(str[index]);
        index += 1;
    }
    return str;
}

int lower_than_string(void* key1, void* key2){
    char* k1=(char*) key1;
    char* k2=(char*) key2;
    if(strcmp(k1,k2)<0) return 1;
    return 0;
}

int lower_than_numeric(void* key1, void* key2){
    double* k1=(double*) key1;
    double* k2=(double*) key2;
    if(*k1 >= *k2) return 1;
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
    libroActual->wordFrecuency = createTreeMap(lower_than_numeric);
    libroActual->wordRelevancy = createTreeMap(lower_than_numeric);
    return libroActual;
}

Library* createBiblioteca(){
    Library* newLibrary = (Library*) malloc(sizeof(Library));
    newLibrary->Libros = createTreeMap( lower_than_string );
    newLibrary->MapCodigo = createMap(150);
    return newLibrary;
}

void procesoArchivo(char archivo[16], char* titulo){
    FILE* fp = NULL;
    fp = fopen ( archivo , "r");//Abrir file

    if(!fp){ //Validar que los archivos se abren correctamente en modo lectura
        printf("Error al abrir el archivo\n");
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
        printf("**HUBO UN ERROR AL ABRIR EL DIRECTORIO**\n");
        printf("**VOLVIENDO AL MENU**\n");
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

// Funcion contarCaracteres // 
/*
    Se encarga de contar los caracteres de una palabra.
    Es una funcion void por lo que no retorna nada.
*/
void contarCaracteres( char* palabraAux, Libro* libro){
    int cont;
    for(cont = 0 ; palabraAux[cont]!= '\0'; cont += 1 ){
        libro->cantCaracter += 1;
    }
    libro->cantCaracter += 1;
}

// Funcion mostrarPalabras // 
/*
    Se encarga de mostrar todas las palabras.
    Es una funcion void por lo que no retorna nada.
*/
void mostrarPalabras(HashMap* MapPalabras){
    Pair* aux = firstMap(MapPalabras);
    Word* palabra;
    while(aux){
        palabra = aux->value;
        printf("%s - %f\n", palabra->palabra, palabra->frecuencia);
        aux = nextMap(MapPalabras);
    }
}

// Funcion removerCaracteresEspeciales // 
/*
    Se encarga de remover caracteres especiales por medio de la tabla ASCII.
    Es una funcion void por lo que no retorna nada.
*/
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
// Funcion LeerArchivo //
/*
  Esta funcion abre un archivo y lee dicho archivo en caso de error vuelve al menu.
  Es una funcion void por lo que no retorna nada.
*/
void LeerArchivo(char* ubicacion, Libro* libro){
    FILE* fp = NULL;
    fp = fopen ( ubicacion , "r");//Abrir file

    if(!fp){ //Validar que los archivos se abren correctamente en modo lectura
        printf("**HUBO UN ERROR AL ABRIR EL ARCHIVO**\n");
        printf("**VOLVIENDO AL MENU**\n");
        system("pause");
        exit (1); 
    }
    ubicarPosicionDeLectura(fp);
    
    Word* palabraAux;
    libro->wordSearch = createMap(250);
    //Aqui obtengo las palabras de cada archivo y las guardo
    char* word = next_word(fp);
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
            palabraAux = createPalabra(AMinuscula(word));
            insertMap(libro->wordSearch, palabraAux->palabra, palabraAux );
            libro->cantPalabra += 1;
        }

        word=next_word(fp);
    }
    fclose(fp);
    char codigo[11];
    quitarFolder(codigo, ubicacion);
    strcpy(libro->codigo, codigo );
}

// Funcion mostrarTitulos //
/*
   Muestra todos los titulos.
   Es una funcion void por lo que no retorna nada
*/
void mostrarTitulos(HashMap* MapLibros){
    Pair* aux = firstMap(MapLibros);
    while(aux){
        printf("%s\n", aux->key);
        aux = nextMap(MapLibros);
    }
}

// Funcion importar //
/*
    Se encarga de agregar libros a la biblioteca hasta que el usuario lo desee, siempre y 
    cuando este libro no este previamente en la biblioteca.
    Es una funcion void por lo que no retrona nada.
*/
void importar(HashMap* MapLibros, Library* biblioteca) {
    system("cls");
    printf("ESTE ES EL LISTADO DE LIBROS DISPONIBLES PARA AGREGAR A LA BIBLIOTECA\n");
    mostrarTitulos(MapLibros);

    char titulo[101];
    printf("\nINGRESE UN TITULO A LA VEZ (SI DESEA SALIR SOLO INGRESE '0'): \n");
    do{
        fflush(stdin);
        gets(titulo);
        if(searchMap(MapLibros, titulo)){
            if( searchTreeMap(biblioteca->Libros, titulo) ){
                printf("**ESTE LIBRO YA FUE AGREGADO A LA BIBLIOTECA**\n");
            }
            else{
                Libro* libroActual = createLibro();
                strcpy(libroActual->titulo, titulo);
                LeerArchivo( (char*)searchMap(MapLibros, titulo)->value , libroActual );
                printf("Titulo del libro: %s\n",libroActual->titulo);
                printf("Codigo del libro: %s\n",libroActual->codigo);
                insertTreeMap(biblioteca->Libros, libroActual->titulo, libroActual); //INSERTAR  LIBRO EN EL TREEMAP DE LA BIBLIOTECA
                insertMap(biblioteca->MapCodigo, libroActual->codigo, libroActual); //INSERTAR LIBRO EN EL HASHMAP DE LA BIBLIOTECA POR SU CODIGO
                printf("**EL LIBRO FUE AGREGADO CON EXITO**\n");
            }
    
        } 
        else{
            if( strcmp(titulo, "0") != 0)
            printf("**SU LIBRO NO FUE ENCONTRADO DENTRO DE NUESTRA BASE DATOS**\n");
        }
    }while(strcmp("0", titulo) != 0);
    system("pause");

}
// Funcion mostrarLibros //
/*
    Se encarga de mostrar todos los libros de la biblioteca y sus datos respectivos
    (codigo,nombre,cantidad de palabras y cantidad de caracteres).
    Es una funcion void por lo que no retrona nada.
*/
void mostrarLibros(Library* biblioteca){
    system("cls");
    printf("\n================ LIBROS DE LA BIBLIOTECA ================\n");
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

// Funcion cantidadDocsConPalabra //
/*
    Se encarga de contar la cantidad de documentos que 
    tienen una palabra específica dada por el usuario.
    Es una funcion double por lo que retorna un numero.
*/
double cantidadDocsConPalabra(Library* biblioteca, char* palabra ){
    double cont = 0;
    PairTree* aux = firstTreeMap(biblioteca->Libros );
    Libro* libroActual;
    while( aux ){
        libroActual = aux->value;
        if(searchMap(libroActual->wordSearch, palabra)){
            cont += 1;
        }
        aux = nextTreeMap( biblioteca->Libros );
    }
    return cont;
}

// Funcion calcularRelevancia //
/*
    Se encargar de calcular la relevancia de una palabra dentro de un libro.
    Es una funcion double por lo que retorna un numero.
*/
double calcularRelevancia(Library* biblioteca, Libro* libro, Word* palabra ){
    double relevancia;
    double docsConLaPalabra= 0;
    docsConLaPalabra = cantidadDocsConPalabra(biblioteca, palabra->palabra);
    relevancia = (palabra->frecuencia / (double) libro->cantPalabra ) * log( (double)biblioteca->MapCodigo->size / docsConLaPalabra); 
    return relevancia;
}

// Funcion obtenerTodasRelevancias //
/*
    Se encarga de obtener las relevancias de todas las palabras de un libro.
    Es una funcion void por lo que no retorna nada.
*/
void obtenerTodasRelevancias(Library* biblioteca, Libro* libro ){
    libro->wordRelevancy = createTreeMap(lower_than_numeric) ;
    Pair* aux = firstMap(libro->wordSearch);
    while(aux){
        Word* palabraActual = aux->value;
        palabraActual->relevancia = calcularRelevancia(biblioteca, libro, palabraActual);

        insertTreeMap(libro->wordRelevancy, &palabraActual->relevancia, palabraActual);
        aux = nextMap(libro->wordSearch);
    }
}

// Funcion palabrasRelevantes //
/*
    Se encarga de mostrar las palabras mas relevantes de un libro indicado por el 
    usuario, muestra mensajes en caso de haber solo un libro o en caso de el libro 
    que esea buscar el usuario no esta dentro de la biblioteca.
    Es una funcion void por lo que no retorna nada.
*/
void palabrasRelevantes(Library* biblioteca){
    system("cls");
    if(biblioteca->MapCodigo->size >= 2){
        printf("\n================ MOSTRAR PALABRAS MAS RELEVANTES ================\n");
        char titulo[101];
        printf("Ingrese un titulo de un Libro: ");
        fflush(stdin);
        gets(titulo);
        PairTree* aux = searchTreeMap(biblioteca->Libros, titulo);
        if ( aux ){
            Libro* libroActual = aux->value;
            obtenerTodasRelevancias(biblioteca, libroActual);
            int cont = 1;
            aux = firstTreeMap(libroActual->wordRelevancy);
            printf("\nLas 10 palabras mas relevantes son:\n");
            while( aux  && cont <= 10){
                Word* palabraActual = aux->value;
                printf("%i.- %s\n", cont ,palabraActual->palabra);
                aux = nextTreeMap(libroActual->wordRelevancy);
                cont += 1;
            }
        }
        else{
            printf("\n**EL LIBRO NO FUE ENCONTRADO EN LA BIBLIOTECA**\n");
        }  
    }
    else{
        printf("\n**PARA CALCULAR LA RELEVANCIA DEBE SER OBLIGATORIAMENTE MAS DE UN LIBRO**\n");
    }
    
    system("pause");
}
// Funcion split //
/*
    XDDD
    Es una funcion List por lo que debe de retornar una lista.
*/
List *split(char *string, char *delim) {
    List *palabras = createList();
    char *token;

    if (string == NULL) return NULL;

    token = strtok(string, delim);
    while (token != NULL) {
        pushBack(palabras, token);
        token = strtok(NULL, delim);
    }

    return palabras;
}

// Funcion verificarLibro //
/*
    Se encarga de verificar si dos palabras de un mismo libro son iguales.
    Es una funcion bool por lo que debe de retornar true o false (1 o 0).
*/
bool verificarLibro(List *words, HashMap *bookWords) {
    char *string1;
    char *string2;
    bool flag;

    string1 = (char *)firstList(words);
    while (string1 != NULL) {
        flag = false;
        Pair* aux = firstMap(bookWords);
        while(aux != NULL) {
            string2 = (char *)((Word *) aux->value)->palabra;
            if (is_equal(string1, string2)) {
                flag = true;
                break;
            } 
            aux = nextMap(bookWords);
        }
        if (flag == false) return false;
        string1 = (char *)nextList(words);
    }

    return true; 
}

// Funcion searhWords //
/*
    Se encarga de buscar un titulo por medio de las palabras ingresadas por el
    usuario.
    Es una funcion void por lo que no retorna nada.
*/
void searchWords(Library *biblioteca) {
    List *words;
    char string[100];
    Library *librosEncontrados = createBiblioteca();
    HashMap *bookWords;
    Libro *book;
    PairTree *aux;
    size_t cont = 1;

    system("cls");
    if(firstTreeMap(biblioteca->Libros)){
        printf("=================== BUSCAR TITULO POR PALABRAS ================\n\n");
        printf("Ingrese las palabras que desee buscar: ");
        fflush(stdin);
        gets(string);
        printf("\n");
        words = split(string, " ");
        book = (Libro *)firstTreeMap(biblioteca->Libros)->value;
        bookWords = book->wordSearch;
        printf("************ TITULO(S) ***********\n");
        while (true) {
            if (verificarLibro(words, bookWords)) {
                printf("%zd.- %s \n", cont, book->titulo);
                cont++;
            }

            aux = nextTreeMap(biblioteca->Libros);
            if (aux == NULL) break;
            book = (Libro *)aux->value;
            bookWords = book->wordSearch;
        }
    }
    else{
        printf("\n**NO HAY LIBROS EN NUESTRA BASE DE DATOS**\n");
    }

    system("pause");
}

// Funcion buscarXPalabra //
/*
    Se encarga de mostrar todos los libros segun la relevancia de una palabra.
    Es una funcion void por lo que no retorna nada.
*/
void buscarXPalabra(Library* biblioteca){
    system("cls");
    if(firstMap(biblioteca->MapCodigo)){
        printf("\n================ MOSTRAR LIBROS SEGUN LA RELEVANCIA DE LA PALABRA ================\n");
        char palabra[21];
        printf("Ingrese una palabra: ");
        fflush(stdin);
        scanf("%s",palabra);
        TreeMap* libroXrelevancia = createTreeMap(lower_than_numeric);
        Pair* aux = firstMap(biblioteca->MapCodigo);//Recorre cada libro
        while(aux){
            Libro* libro = aux->value;
            aux = searchMap( libro->wordSearch, palabra);//Busca la palabra
            if( aux ){
                double* relevancia = (double*) malloc(sizeof(double));
                *relevancia = calcularRelevancia(biblioteca, libro, (Word*)aux->value);//Calcular la relevancia de la palabra
                insertTreeMap(libroXrelevancia, relevancia, libro);//Insertando ese libro segun la relevancia obtenida
            }
            aux = nextMap(biblioteca->MapCodigo);
        }

        PairTree* aux2 = firstTreeMap(libroXrelevancia); //Recorriendo todos los libro en orden de mayor relevancia de la palabra
        if(aux2){
        while(aux2){
                Libro* libro = aux2->value;
                printf("Codigo del libro: %s\n", libro->codigo);
                printf("Titulo del libro: %s\n\n", libro->titulo);
                aux2 = nextTreeMap(libroXrelevancia);
            } 
        }
        else{
            printf("\n**NO EXISTE ALGUN LIBRO EN NUESTRA BASE DE DATOS QUE CONTENGA ESA PALABRA**\n");
        } 
    }
    else{
        printf("\n**NO HAY LIBROS EN NUESTRA BASE DE DATOS**\n");
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
        printf("3.-  Buscar titulos por palabras\n");
        printf("4.-  10 Palabras con mayor frecuencia\n");  
        printf("5.-  10 Palabras mas relevantes\n"); 
        printf("6.-  Buscar libros por relevancia de la palabra\n");  
        printf("7.-  Mostrar palabra en su contexto dentro del libro\n");
        printf("8.-  Salir\n");

        printf("\nINGRESE SU OPCION: ");

        while (true) {
            fflush(stdin);
            gets(opcion);
            if (esNumero(opcion)) break;
            else printf("**OPCION INCORRECTA POR FAVOR INGRESELO NUEVAMENTE**");
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
            case 3: 
                searchWords( biblioteca );
                break;
            /*case 4: 
                palabrasFrecuentes( biblioteca );
                break;*/
            case 5: 
                palabrasRelevantes( biblioteca );
                break;
            case 6: 
                buscarXPalabra( biblioteca );
                break;
            /*case 7: 
                mostrarEnContexto( biblioteca );
                break;*/
            case 8:
                return EXIT_SUCCESS;
        }
    }

    return EXIT_SUCCESS;
}

