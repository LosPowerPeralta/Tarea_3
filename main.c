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
    List *pos;
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

// Funcion AMinuscula // 
/*
    Se encarga de pasar una letra mayuscula a minuscula.
    Es de tipo char por lo que debe retorna una letra.
*/
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

// Funcion de TreeMap //
// Compara cadenas en orden alfabética
int lower_than_string(void* key1, void* key2){
    char* k1=(char*) key1;
    char* k2=(char*) key2;
    if(strcmp(k1,k2)<0) return 1;
    return 0;
}

// Funcion de TreeMap //
// Compara numeros de mayor a menor
int lower_than_numeric(void* key1, void* key2){
    double* k1=(double*) key1;
    double* k2=(double*) key2;
    if(*k1 >= *k2) return 1;
    return 0;
}

// Funcion createPalabra //
/*
    Se encarga de crear una variable de tipo palabra y de inicializar
    sus variables.
    Es una funcion de tipo Word por lo que debe retornar una variable puntero de tipo Word.
*/
Word* createPalabra(char* str, List *pos){
    Word* NewWord = (Word*) malloc( sizeof(Word) );
    NewWord->palabra = (char*) malloc( strlen(str)+1);
    strcpy(NewWord->palabra,str);
    NewWord->frecuencia = 1;
    NewWord->pos = pos;
    return NewWord;
}

// Funcion createLibro //
/*
    Se encarga de crear una variable de tipo Libro y de inicializar
    sus variables.
    Es una funcion de tipo Libro por lo que debe retornar una variable puntero de tipo Libro.
*/
Libro* createLibro(){
    Libro* libroActual = (Libro*) malloc(sizeof(Libro));
    libroActual->cantCaracter = 0;
    libroActual->cantPalabra = 0;
    libroActual->wordFrecuency = createTreeMap(lower_than_numeric);
    libroActual->wordRelevancy = createTreeMap(lower_than_numeric);
    return libroActual;
}

// Funcion createBiblioteca //
/*
    Se encarga de crear una variable de tipo Library y de inicializar
    sus variables.
    Es una funcion de tipo Library por lo que debe retornar una variable puntero de tipo Library.
*/
Library* createBiblioteca(){
    Library* newLibrary = (Library*) malloc(sizeof(Library));
    newLibrary->Libros = createTreeMap( lower_than_string );
    newLibrary->MapCodigo = createMap(150);
    return newLibrary;
}

// Funcion procesoArchivo//
// Obtiene el titulo de todos los archivos para listarlos
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

// Funcion esNumero //
/*
    Corrobora si el caracter es o no un numero.
    Es una funcion de tipo bool por lo que debe retornar un true o false (1 o 0).
*/
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

// Funcion listarArchivos //
// Abre todos los archivos de la carpeta libros para guardar sus nombres y listarlos
// Hace más fácil la función importar para el usuario
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

// Funcion quitarfolder //
// De una ubicación de archivo /Libros/xxx.txt corta el string para obtener el código
// xxx queda de esta forma y se guarda en codigo
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

// Funcion next_word //
// Obtiene la siguiente palabra del archivo.txt y la retorna
char* next_word (FILE *f) {
    char x[1024];
    if (fscanf(f, " %1023s", x) == 1) {
        return strdup(x);
    }
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

// Funcion ubicar posicion de lectura //
// Ubica el puntero para leer el archivo en donde exactamente comienza el texto
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
    long long *auxPos;
    List *pos;
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
        auxPos = (long long *) malloc(sizeof(long long));
        *auxPos = ftell(fp);
        aux = searchMap(libro->wordSearch, word);
        if( aux ){
           palabraAux = aux->value;
           pushBack(palabraAux->pos, auxPos);
           palabraAux->frecuencia += 1; 
        }
        else{
            pos = createList();
            pushBack(pos, auxPos);
            contarCaracteres(word, libro);
            removerCaracteresEspeciales(word);
            palabraAux = createPalabra(AMinuscula(word), pos);
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

/************ | función split() | ****************/
/* El proposito de esta función es guardar todas *
 * las cadenas de caracteres que se encuentren   *
 * separadas por el caracter recibido.           *
 * Esta función recibe una cadena de caracteres  *
 * y el caracter que separa cada parte de la     *
 * cadena.                                       *
 * Retorna una lista con las palabras separadas  */
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

/******************* | función verificarLibro() | ********************/
/* El proposito de esta función es revisar si las palabbras          *
 * ingresadas por el usuario se encuentran dentro del libro recibido.*
 * Recibe una lista con las palabras y el libro a revisar.           *
 * Retorna true si es que todas las palabras se encuentran dentro    *
 * del libro y false en caso contrario.                              */
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

/*************** | funcion searchWords() | *****************/
/* El proposito de esta función es buscar todos los libros *
 * que contengan las palabras ingresadas por el usuario.   *
 * Recibe una variable que contiene todos los libros       *
 * almacenados.                                            *
 * No retorna valores.                                     */
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

// Funcion palabrasFrecuentes //
/* 
    Se encarga de mostrar las palabras mas frecuentes de un libro indicado por el 
    usuario, muestra mensajes en caso de haber libros o en caso de que el libro 
    que desea buscar el usuario no esta dentro de la biblioteca.
    Imprime las 10 palabras mas frecuentes.
*/
void palabrasFrecuentes(Library* biblioteca){                   //Función que imprime las palabras más frecuentes del libro que se indique
    double * calcularFrecuencia;
    Word* palabraActual;
    system("cls");
    printf("\nMOSTRAR PALABRAS MAS FRECUENTES\n");
    char titulo[101];
    printf("Ingrese un titulo de un Libro: ");
    gets(titulo);
    PairTree* auxT = searchTreeMap(biblioteca->Libros, titulo);     //auxT = Arbol binario ocupado para ordenar frecuencias de mayor a menor
    if ( auxT ){                                                    //Si el libro ingresado existe
        Libro* libroActual = auxT->value;
        Pair* aux = firstMap(libroActual->wordSearch);
        while (aux){                                                //Mientras el mapa no termine
            Word* palabraActual = (Word*) aux->value;
            calcularFrecuencia = (double *) malloc (sizeof(double));        
            *calcularFrecuencia = palabraActual->frecuencia / libroActual->cantPalabra;     //Se crea la variable que calcula la frecuencia 
            insertTreeMap(libroActual->wordFrecuency, calcularFrecuencia, palabraActual);   //de cada palabra en el libro
        }
        int cont = 1;
        auxT = firstTreeMap(libroActual->wordFrecuency);
        printf("\nLas 10 palabras mas frecuentes son:\n");
        while( auxT  && cont <= 10){                                 //Se imprimiran las 10 primeras palabras ordenadas 
            palabraActual = (Word*) auxT->value;
            printf("%i.- %s  \n", cont ,palabraActual->palabra);
            auxT = nextTreeMap(libroActual->wordFrecuency);
            cont += 1;
        }
    }
    else{
        printf("\nEl libro no se encuentra en la biblioteca\n");       //En caso de ingresar un libro inválido imprime el msg
    }
    system("pause");

}

/************************* | funcion mostrarEnContexto() | *************************/
/* El proposito de esta función es mostrar en que contexto esta colocada la        *
 * palabra escojida por el usuario.                                                *
 * Recibe un mapa que contiene todos los nombres de los libros y otro que contiene *
 * todo el contenido dentro de los mismos.                                         *
 * No retorna valores.                                                             */
void mostrarEnContexto(HashMap *MapArchivos, HashMap *MapCodigos) {
    char titulo[50];
    Pair *aux;
    char codigo[10];
    char opcion[1];
    char word[50];
    char linea[1024];
    Libro *libroRevisado;
    Word *palabraEncontrada;
    FILE *libro;
    long long *pos;
    int cont = 1;

    system("cls");
    printf("================= MOSTRAR CONTEXTO ======================\n\n");
    printf("Que libro desea ver?: ");
    fflush(stdin);
    gets(titulo);

    if ((aux = searchMap(MapArchivos, titulo)) != NULL) {
        libro = fopen(aux->value, "r");
        quitarFolder(codigo, (char *)aux->value);  
        if ((aux = searchMap(MapCodigos, codigo)) == NULL) {
            printf("El libro no fue importado, desea intentarlo de nuevo? (s/n): ");
            while (true) {
                fflush(stdin);
                gets(opcion);
                strcpy(opcion, AMinuscula(opcion));
                if (opcion[0] == 's') {
                    mostrarEnContexto(MapArchivos, MapCodigos);
                }
                if (opcion[0] == 'n') return;
                printf("Opcion escojida incorrecta, intentelo de nuevo: ");
            }
        }
    }
    else {
        printf("El libro no existe, desea intentarlo de nuevo? (s/n): ");
        while (true) {
            fflush(stdin);
            gets(opcion);
            strcpy(opcion, AMinuscula(opcion));
            if (opcion[0] == 's') {
                mostrarEnContexto(MapArchivos, MapCodigos);
            }
            if (opcion[0] == 'n') return;
            printf("Opcion escojida incorrecta, intentelo de nuevo: ");
        }
    }
    printf("Que palabra desea buscar?: ");
    while (true) {
        fflush(stdin);
        gets(word);

        aux = searchMap(MapCodigos, codigo);
        libroRevisado = (Libro *)aux->value;
        if (searchMap(libroRevisado->wordSearch, word) == NULL) {
            printf("Palabra elegida no existe, desea intentarlo de nuevo? (s\n): ");
            while (true) {
                fflush(stdin);
                gets(opcion);
                
                strcpy(opcion, AMinuscula(opcion));
                if (opcion[0] == 'n') return;
                if (opcion[0] == 's') break;
                printf("Opcion escojida incorrecta, intentelo de nuevo: ");
            }
        }
        else break;
    }
    printf("\n");
    palabraEncontrada = (Word *)searchMap(libroRevisado->wordSearch, word)->value;
    pos = (long long *)firstList(palabraEncontrada->pos);
    while(pos != NULL) {
        fseek(libro, (*pos) - 40, SEEK_SET);
        fscanf(libro, "%1023s", linea);
        printf("%d.- ", cont);
        while(ftell(libro) < (*pos) + 40) {
            fscanf(libro, "%1023s", linea);
            printf("%s ", linea);
        }
        printf("\n");
        cont++;
        pos = (long long *)nextList(palabraEncontrada->pos);
    }
    printf("\n");
    printf("===================== VOLVIENDO AL MENU ======================\n");
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
            case 4: 
                palabrasFrecuentes( biblioteca );
                break;
            case 5: 
                palabrasRelevantes( biblioteca );
                break;
            case 6: 
                buscarXPalabra( biblioteca );
                break;
            case 7: 
                mostrarEnContexto( MapArchivos, biblioteca->MapCodigo );
                break;
            case 8:
                return EXIT_SUCCESS;
        }
    }

    return EXIT_SUCCESS;
}

