#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <time.h>

#include "tp.h"
#include "abb.h"
#include "abb_estructura_privada.h"
#include "lista.h"

enum TP_JUGADOR { JUGADOR_1, JUGADOR_2 };

enum TP_OBSTACULO {
	OBSTACULO_FUERZA,
	OBSTACULO_DESTREZA,
	OBSTACULO_INTELIGENCIA
};
#define IDENTIFICADOR_OBSTACULO_FUERZA 'F'

#define IDENTIFICADOR_OBSTACULO_DESTREZA 'D'

#define IDENTIFICADOR_OBSTACULO_INTELIGENCIA 'I'


#define LONGITUD_BASE 50

#define OBSTACULOS_FACIL 3

#define OBSTACULOS_MEDIO 5

#define OBSTACULOS_DIFICIL 7

#define OBSTACULOS_IMPOSIBLE 10

typedef struct {
    int longitud_pista;
    lista_t *pista_jugador;
    lista_t *pista_bot;
    lista_t *obstaculos_jugador;
    lista_t *obstaculos_bot;
} pista_carreras_t;

struct tp {
    abb_t *abb_pokemon;
    struct pokemon_info *seleccionado_jugador[2]; // primera pos es siempre el jugador, segunda pos es siempre el bot
    pista_carreras_t *pista;
    int dificultad; // 0: Facil, 1: Medio, 2:Dificil, 3:Imposible 
};

// Estructura para el contexto de iteración
struct datos_de_iteracion {
    size_t iteracion_actual;
    size_t iteracion_objetivo;
    const struct pokemon_info **pokemon_seleccionado;
};

/*INCOMPLETO:

1°) Inicializar la pista del jugador con "-"
2°) Hacer tp_agregar_obstaculo


*/ 



//Función para comparar pokemones al momento de insertarlos en el abb
int comparar_pokemon(void *a, void *b) {
    struct pokemon_info *p1 = (struct pokemon_info *)a;
    struct pokemon_info *p2 = (struct pokemon_info *)b;
    return strcasecmp(p1->nombre, p2->nombre);
}

TP *tp_crear(const char *nombre_archivo)
{
    TP *nuevo_tp = calloc(1, sizeof(TP));
    if (!nuevo_tp) {
        printf("No se pudo asignar memoria para el TP\n");
        return NULL;
    }

    FILE *archivo = fopen(nombre_archivo, "r");
    if (!archivo) {
        fprintf(stderr, "No se pudo abrir el archivo %s\n", nombre_archivo);
        free(nuevo_tp);
        return NULL;
    }

    nuevo_tp->abb_pokemon = abb_crear(comparar_pokemon);
    if (!nuevo_tp->abb_pokemon) {
        fprintf(stderr, "No se pudo crear el ABB\n");
        fclose(archivo);
        free(nuevo_tp);
        return NULL;
    }

    char linea[256];
    while (fgets(linea, sizeof(linea), archivo)) {
        struct pokemon_info *pokemon = calloc(1, sizeof(struct pokemon_info));
        if (!pokemon) {
            fprintf(stderr, "Error: No se pudo asignar memoria para el Pokémon\n");
            fclose(archivo);
            abb_destruir(nuevo_tp->abb_pokemon);
            free(nuevo_tp);
            return NULL;
        }

        char *nombre = strtok(linea, ",");
        char *fuerza = strtok(NULL, ",");
        char *destreza = strtok(NULL, ",");
        char *inteligencia = strtok(NULL, ",");

        if (nombre && fuerza && destreza && inteligencia) {
            pokemon->nombre = strdup(nombre);
            pokemon->fuerza = atoi(fuerza);
            pokemon->destreza = atoi(destreza);
            pokemon->inteligencia = atoi(inteligencia);

            abb_insertar(nuevo_tp->abb_pokemon, pokemon);
        } else {
            fprintf(stderr, "Error: Formato de línea incorrecto\n");
            free(pokemon);
        }
    }
    fclose(archivo);

    nuevo_tp->pista = calloc(1, sizeof(pista_carreras_t));
    if (!nuevo_tp->pista) {
        fprintf(stderr, "No se pudo asignar memoria para la pista\n");
        abb_destruir(nuevo_tp->abb_pokemon);
        free(nuevo_tp);
        return NULL;
    }

	nuevo_tp->pista->obstaculos_jugador = lista_crear();
    nuevo_tp->pista->obstaculos_bot = lista_crear();

    if (!nuevo_tp->pista->obstaculos_jugador || !nuevo_tp->pista->obstaculos_bot) {
        fprintf(stderr, "No se pudo crear las listas de obstáculos\n");
        abb_destruir(nuevo_tp->abb_pokemon);
        lista_destruir(nuevo_tp->pista->obstaculos_jugador);
        lista_destruir(nuevo_tp->pista->obstaculos_bot);
        free(nuevo_tp->pista);
        free(nuevo_tp);
        return NULL;
    }

    return nuevo_tp;
}

// Función para crear una pista e inicializarla con el char "-"
lista_t *pista_crear_e_inicializar(int longitud) {

    lista_t *pista = lista_crear();
    if (!pista) {
        fprintf(stderr, "No se pudo crear la lista para la pista\n");
        return NULL;
    }

    for (int i = 0; i < longitud; i++) {
        char *caracter = malloc(sizeof(char));
        if (!caracter) {
            fprintf(stderr, "No se pudo asignar memoria para el carácter\n");
            lista_destruir(pista);  // Destruir la lista antes de retornar NULL
            return NULL;
        }
        *caracter = '-';
        lista_insertar(pista, caracter);
    }

    return pista;
}

void insertar_obstaculos_pista_bot(TP *tp) {
    if (!tp || !tp->pista)
        return;

    int longitud_pista = tp->pista->longitud_pista;

    // Determinar la cantidad de obstáculos según la dificultad
    int cantidad_obstaculos = 0;
    switch (tp->dificultad) {
        case 0: cantidad_obstaculos = OBSTACULOS_FACIL; break;  // Fácil
        case 1: cantidad_obstaculos = OBSTACULOS_MEDIO; break;  // Medio
        case 2: cantidad_obstaculos = OBSTACULOS_DIFICIL; break;  // Difícil
        case 3: cantidad_obstaculos = OBSTACULOS_IMPOSIBLE; break;  // Imposible
        default:
            fprintf(stderr, "Dificultad inválida\n");
            return;
    }

    // Inicializar randomizador
    srand(time(NULL));

    // Generar obstáculos aleatorios
    for (int i = 0; i < cantidad_obstaculos; i++) {

        // Determinar tipo de obstáculo (fuerza, destreza, inteligencia)
        enum TP_OBSTACULO tipo_obstaculo = rand() % 3;

        // Generar posición aleatoria en la longitud de la pista con al menos 5 espacios libres
        int min_posicion = 5;
        int max_posicion = longitud_pista - 5;
        int posicion = min_posicion + rand() % (max_posicion - min_posicion + 1);

        // Agregar obstáculo a la pista del bot
        tp_agregar_obstaculo(tp, COMPUTADORA, tipo_obstaculo, posicion);
    }
}

TP *inicializar_juego(TP *tp, int dificultad) {
    if (tp == NULL || dificultad < 0 || dificultad > 3)
        return NULL;

    // Inicializar la dificultad
    tp->dificultad = dificultad;

    // Inicializar longitud de la pista según la dificultad
    tp->pista->longitud_pista = LONGITUD_BASE + (dificultad * 25);

    // Crear las pistas vacías/Inicializa las pistas con el caracter "-"
    tp->pista->pista_jugador = pista_crear_e_inicializar(tp->pista->longitud_pista);
    tp->pista->pista_bot = pista_crear_e_inicializar(tp->pista->longitud_pista);

    // Verificación de que no haya fallado ningún calloc
    if (!tp->pista->pista_jugador || !tp->pista->pista_bot) {
        fprintf(stderr, "No se pudo crear las pistas\n");
        lista_destruir(tp->pista->pista_jugador);
        lista_destruir(tp->pista->pista_bot);
        return NULL;
    }

    //Inserta obstaculos de forma aleatoria en la pista del bot
    tp->pista->pista_bot = insertar_obstaculos_pista_bot(tp);

    return tp;
}

unsigned tp_agregar_obstaculo(TP *tp, enum TP_JUGADOR jugador, enum TP_OBSTACULO tipo, unsigned posicion) {
    lista_t *pista = (jugador == JUGADOR_1) ? tp->pista->pista_jugador : tp->pista->pista_bot;
    lista_t *obstaculos_lista = (jugador == JUGADOR_1) ? tp->pista->obstaculos_jugador : tp->pista->obstaculos_bot;

    // Obtener el carácter correspondiente al tipo de obstáculo
    char *obstaculo_char = malloc(sizeof(char));
    if (!obstaculo_char) {
        fprintf(stderr, "No se pudo asignar memoria para el carácter\n");
        return 0;
    }
    switch (tipo) {
        case OBSTACULO_FUERZA:
            *obstaculo_char = IDENTIFICADOR_OBSTACULO_FUERZA;
            break;
        case OBSTACULO_DESTREZA:
            *obstaculo_char = IDENTIFICADOR_OBSTACULO_DESTREZA;
            break;
        case OBSTACULO_INTELIGENCIA:
            *obstaculo_char = IDENTIFICADOR_OBSTACULO_INTELIGENCIA;
            break;
        default:
            fprintf(stderr, "Error: Tipo de obstáculo inválido\n");
            free(obstaculo_char);
            return 0;
    }

    if (jugador == COMPUTADORA) {
        // Verificar que no haya un obstáculo en la posición antes de insertar
        while (*(char *)lista_elemento_en_posicion(pista, posicion) != '-') {
            posicion = (posicion + 1) % lista_tamanio(pista);
        }
        lista_insertar_en_posicion(pista, obstaculo_char, posicion);

        // Agregar el obstáculo a la lista de obstáculos del bot
        lista_insertar(obstaculos_lista, obstaculo_char);

        return lista_tamanio(obstaculos_lista);
    } else {
        // Verificar si la posición es mayor a la longitud de la pista
        if (posicion >= lista_tamanio(pista)) {
            posicion = lista_tamanio(pista) - 1;
        }

        // Insertar el obstáculo en la posición especificada de la pista
        if (*(char *)lista_elemento_en_posicion(pista, posicion) != '-') {
            lista_insertar_en_posicion(pista, obstaculo_char, posicion + 1);
        } else {
            lista_insertar_en_posicion(pista, obstaculo_char, posicion);
        }

        // Agregar el obstáculo a la lista de obstáculos del jugador
        lista_insertar(obstaculos_lista, obstaculo_char);

        return lista_tamanio(obstaculos_lista);
    }
}

unsigned tp_quitar_obstaculo(TP *tp, enum TP_JUGADOR jugador, unsigned posicion) {
    if (!tp || !tp->pista)
        return 0;

    lista_t *pista = (jugador == JUGADOR_1) ? tp->pista->pista_jugador : tp->pista->pista_bot;
    lista_t *obstaculos_lista = (jugador == JUGADOR_1) ? tp->pista->obstaculos_jugador : tp->pista->obstaculos_bot;

    // Verificar si la posición es válida
    if (posicion >= lista_tamanio(pista)) {
        fprintf(stderr, "Posición inválida\n");
        return 0;
    }

    // Obtener el elemento en la posición especificada
    char *elemento = (char *)lista_elemento_en_posicion(pista, posicion);

    // Verificar si hay un obstáculo en la posición
    if (!elemento || *elemento == '-') {
        fprintf(stderr, "No hay obstáculo en la posición especificada\n");
        return 0;
    }

    // Eliminar el obstáculo y reemplazarlo con '-'
    *elemento = '-';

    // Remover el obstáculo de la lista de obstáculos
    for (size_t i = 0; i < lista_tamanio(obstaculos_lista); i++) {
        char *obstaculo = (char *)lista_elemento_en_posicion(obstaculos_lista, i);
        if (*obstaculo == *elemento) {
            lista_quitar_de_posicion(obstaculos_lista, i);
            free(obstaculo);
            break;
        }
    }

    return lista_tamanio(obstaculos_lista);
}

// Función de intercambio
void intercambiar(char *x, char *y) {
    char temp = *x;
    *x = *y;
    *y = temp;
}

// Implementación de Bubble Sort
void ordenamiento_burbuja(char *arr, int n) {
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (arr[j] > arr[j+1]) {
                intercambiar(&arr[j], &arr[j+1]);
            }
        }
    }
}

char *tp_obstaculos_pista(TP *tp, enum TP_JUGADOR jugador) {
    if (!tp || !tp->pista)
        return NULL;

    lista_t *obstaculos_lista = (jugador == JUGADOR_1) ? tp->pista->obstaculos_jugador : tp->pista->obstaculos_bot;

    if (lista_tamanio(obstaculos_lista) == 0) {
        return NULL;
    }

    // Crear un array para almacenar los obstáculos
    size_t tamanio = lista_tamanio(obstaculos_lista);
    char *obstaculos = malloc((tamanio + 1) * sizeof(char));
    if (!obstaculos) {
        fprintf(stderr, "No se pudo asignar memoria para los obstáculos\n");
        return NULL;
    }

    // Copiar los obstáculos a un array
    for (size_t i = 0; i < tamanio; i++) {
        char *obstaculo = (char *)lista_elemento_en_posicion(obstaculos_lista, i);
        obstaculos[i] = *obstaculo;
    }

    // Ordenar los obstáculos usando bubble sort
    bubble_sort(obstaculos, tamanio);

    // Asegurarse de que el string esté null-terminated
    obstaculos[tamanio] = '\0';

    return obstaculos;
}

void tp_limpiar_pista(TP *tp, enum TP_JUGADOR jugador) {
    if (!tp || !tp->pista)
        return;

    lista_t *pista = (jugador == JUGADOR_1) ? tp->pista->pista_jugador : tp->pista->pista_bot;
    lista_t *obstaculos = (jugador == JUGADOR_1) ? tp->pista->obstaculos_jugador : tp->pista->obstaculos_bot;

    // Limpiar la pista
    for (size_t i = 0; i < lista_tamanio(pista); i++) {
        char *elemento = (char *)lista_elemento_en_posicion(pista, i);
        if (elemento && *elemento != '-') {
            *elemento = '-';
        }
    }

    // Limpiar la lista de obstáculos
    lista_destruir(obstaculos);
    if (jugador == JUGADOR_1) {
        tp->pista->obstaculos_jugador = lista_crear();
    } else {
        tp->pista->obstaculos_bot = lista_crear();
    }
}

unsigned tp_calcular_tiempo_pista(TP *tp, enum TP_JUGADOR jugador)
{
	return 0;
}

char *tp_tiempo_por_obstaculo(TP *tp, enum TP_JUGADOR jugador)
{
	return NULL;
}

void tp_destruir(TP *tp)
{
	return;
}






//Funciones sobre pokemones 
int tp_cantidad_pokemon(TP *tp)
{
	if (tp == NULL){
		return 0;
	}

	return (int)(tp->abb_pokemon->tamanio);
}

const struct pokemon_info *tp_buscar_pokemon(TP *tp, const char *nombre) {
    if (tp == NULL || nombre == NULL) {
        return NULL;
    }

    struct pokemon_info pokemon_a_buscar;
    pokemon_a_buscar.nombre = (char *)nombre;

    struct pokemon_info *resultado = abb_buscar(tp->abb_pokemon, &pokemon_a_buscar);

    return resultado;
}

bool concatenar_nombres(void *elemento, void *aux) {
    struct pokemon_info *pokemon = (struct pokemon_info *)elemento;
    char **nombres = (char **)aux;

    if (*nombres == NULL) {
        *nombres = malloc(strlen(pokemon->nombre) + 1);
        if (*nombres == NULL) 
        	return false;
        strcpy(*nombres, pokemon->nombre);
    } else {
        size_t len_actual = strlen(*nombres);
        size_t len_nuevo = len_actual + strlen(pokemon->nombre) + 2; // +2 para la coma y el /0
        char *temp = realloc(*nombres, len_nuevo);
        if (temp == NULL) 
        	return false;
        *nombres = temp;
        strcat(*nombres, ",");
        strcat(*nombres, pokemon->nombre);
    }
    return true;
}

char *tp_nombres_disponibles(TP *tp) {
    if (tp == NULL || tp->abb_pokemon == NULL) {
        return NULL;
    }

    char *nombres = NULL;
    abb_con_cada_elemento(tp->abb_pokemon, INORDEN, concatenar_nombres, &nombres);
    return nombres;
}

bool seleccionar_pokemon_en_iteracion(void *elemento, void *contexto) {
    struct datos_de_iteracion *datos = contexto;

    if (datos->iteracion_actual == datos->iteracion_objetivo) {
        *(datos->pokemon_seleccionado) = elemento;
        return false; // Detener la iteración
    }

    datos->iteracion_actual++;
    return true; // Continuar la iteración
}

bool tp_seleccionar_pokemon(TP *tp, enum TP_JUGADOR jugador, const char *nombre){
    if (tp == NULL || tp->abb_pokemon == NULL) {
        return false;
    }

    const struct pokemon_info *pokemon_jugador = tp_buscar_pokemon(tp, nombre);
    if (pokemon_jugador == NULL) {
        return false;
    }

    // Verificar que no se haya seleccionado el mismo Pokémon para ambos jugadores
    if (tp->seleccionado_jugador[COMPUTADORA] != NULL &&
        strcmp(pokemon_jugador->nombre, tp->seleccionado_jugador[COMPUTADORA]->nombre) == 0) {
        return false;
    }

    tp->seleccionado_jugador[JUGADOR_1] = (struct pokemon_info *)pokemon_jugador;

    size_t cantidad_pokemones = (size_t)tp_cantidad_pokemon(tp);

    // Seleccionar un Pokémon aleatorio para la computadora
    srand((unsigned int)time(NULL));
    size_t iteracion_objetivo = (size_t)(rand() % (int)cantidad_pokemones);
    const struct pokemon_info *pokemon_computadora = NULL;

    struct datos_de_iteracion datos = {0, iteracion_objetivo, &pokemon_computadora};

    do {
        abb_con_cada_elemento(tp->abb_pokemon, INORDEN, seleccionar_pokemon_en_iteracion, &datos);
    } while (pokemon_computadora != NULL &&
             strcmp(pokemon_computadora->nombre, tp->seleccionado_jugador[JUGADOR_1]->nombre) == 0);

    tp->seleccionado_jugador[COMPUTADORA] = (struct pokemon_info *)pokemon_computadora;

    return true;
}

const struct pokemon_info *tp_pokemon_seleccionado(TP *tp,
						   enum TP_JUGADOR jugador)
{
	if(tp == NULL){
		return NULL;
	}
	return tp->seleccionado_jugador[JUGADOR_1];
}