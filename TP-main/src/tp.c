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

#define JUGADOR_1 0
#define COMPUTADORA 1
#define LONGITUD_BASE 50
#define IDENTIFICADOR_OBSTACULO_FUERZA 'F'
#define IDENTIFICADOR_OBSTACULO_DESTREZA 'D'
#define IDENTIFICADOR_OBSTACULO_INTELIGENCIA 'I'
#define OBSTACULOS_FACIL 3
#define OBSTACULOS_MEDIO 5
#define OBSTACULOS_DIFICIL 7
#define OBSTACULOS_IMPOSIBLE 10

typedef struct {
    int longitud_pista;
    lista_t *pista_jugador;
    lista_t *pista_bot;
    lista_t *obstaculos_lista;
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

1°) Inicializar la pista del jugador y bot con "-"
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

    nuevo_tp->pista->obstaculos_lista = lista_crear();
    if (!nuevo_tp->pista->obstaculos_lista) {
        fprintf(stderr, "No se pudo crear la lista de obstáculos\n");
        abb_destruir(nuevo_tp->abb_pokemon);
        free(nuevo_tp->pista);
        free(nuevo_tp);
        return NULL;
    }
    return nuevo_tp;
}

// Función para crear una pista vacía con la longitud especificada
lista_t *pista_crear_vacia(int longitud) {
    lista_t *pista = lista_crear();
    if (!pista) {
        fprintf(stderr, "No se pudo crear la lista para la pista\n");
        return NULL;
    }

    for (int i = 0; i < longitud; i++) {
        lista_insertar(pista, NULL);
    }

    return pista;
}

void inicializar_pistas_con_obstaculos(TP *tp) {
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
        char *obstaculo_char = malloc(sizeof(char));

        // Insertar el caracter correspondiente en la lista de obstáculos
        switch (tipo_obstaculo) {
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
                return;
        }

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

    // Crear las pistas vacías
    tp->pista->pista_jugador = pista_crear_vacia(tp->pista->longitud_pista);
    tp->pista->pista_bot = pista_crear_vacia(tp->pista->longitud_pista);

    // Verificación de que no haya fallado ningún calloc
    if (!tp->pista->pista_jugador || !tp->pista->pista_bot) {
        fprintf(stderr, "No se pudo crear las pistas\n");
        lista_destruir(tp->pista->pista_jugador);
        lista_destruir(tp->pista->pista_bot);
        free(tp->pista);
        return NULL;
    }

    //Inicializa las pistas con el caracter "-"
    tp->pista->pista_bot = inicializar_pista();
    tp->pista->pista_jugador = inicializar_pista();

    //Inserta obstaculos de forma aleatoria
    tp->pista->pista_bot = inicializar_pistas_con_obstaculos(tp);

    return tp;
}

// Función para agregar un obstáculo en la pista de un jugador
void tp_agregar_obstaculo(TP *tp, enum TP_JUGADOR jugador, enum TP_OBSTACULO tipo, unsigned posicion) {

}

unsigned tp_quitar_obstaculo(TP *tp, enum TP_JUGADOR jugador, unsigned posicion)
{
	return 0;
}

char *tp_obstaculos_pista(TP *tp, enum TP_JUGADOR jugador)
{
	return NULL;
}

void tp_limpiar_pista(TP *tp, enum TP_JUGADOR jugador)
{
	return;
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
