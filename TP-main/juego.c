#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/tp.h"

#include "src/abb.h"
#include "src/abb_estructura_privada.h"
#include "src/lista.h"

#define JUGADOR_1 0
#define COMPUTADORA 1

typedef struct {
    struct pokemon_info *pokemones[2];
    int posiciones[2];
    int longitud_pista;
    lista_t *pista_jugador;
    lista_t *pista_bot;
} pista_carreras_t;

struct tp {
    abb_t *abb_pokemon;
    struct pokemon_info *seleccionado_jugador[2];
    pista_carreras_t *pista;
    int dificultad;
};

struct datos_de_iteracion {
    size_t iteracion_actual;
    size_t iteracion_objetivo;
    const struct pokemon_info **pokemon_seleccionado;
};

bool imprimir_pokemon(void *elemento, void *aux) {
    struct pokemon_info *pokemon = (struct pokemon_info *)elemento;
    printf("Nombre: %s, Fuerza: %d, Destreza: %d, Inteligencia: %d\n",
           pokemon->nombre, pokemon->fuerza, pokemon->destreza, pokemon->inteligencia);
    return true;
}

void destruir_pokemon(void *elemento) {
    struct pokemon_info *pokemon = (struct pokemon_info *)elemento;
    free(pokemon->nombre);
    free(pokemon);
}

bool imprimir_elemento(void *elemento, void *contexto) {
    printf("%c", *(char *)elemento);
    return true;
}

void imprimir_pista(lista_t *pista) {
    if (!pista) return;
    lista_con_cada_elemento(pista, imprimir_elemento, NULL);
    printf("\n");
}

void probar_carga_y_busqueda_abb(const char *nombre_archivo, const char *nombre_pokemon_buscar) {
    TP *tp = tp_crear(nombre_archivo);
    if (!tp) {
        fprintf(stderr, "Error al crear el TP\n");
        return;
    }

    printf("Pokémones en el ABB:\n");
    abb_con_cada_elemento(tp->abb_pokemon, INORDEN, imprimir_pokemon, NULL);

    int cantidad = tp_cantidad_pokemon(tp);
    printf("Cantidad de Pokémon en el ABB: %d\n", cantidad);

    printf("Buscando Pokémon por nombre: %s\n", nombre_pokemon_buscar);
    const struct pokemon_info *pokemon_encontrado = tp_buscar_pokemon(tp, nombre_pokemon_buscar);
    
    if (pokemon_encontrado) {
        printf("Pokémon encontrado:\n");
        printf("Nombre: %s, Fuerza: %d, Destreza: %d, Inteligencia: %d\n",
               pokemon_encontrado->nombre, pokemon_encontrado->fuerza,
               pokemon_encontrado->destreza, pokemon_encontrado->inteligencia);
    } else {
        printf("Pokémon no encontrado.\n");
    }

    char *nombres_disponibles = tp_nombres_disponibles(tp);
    if (nombres_disponibles) {
        printf("Nombres de Pokémon disponibles: %s\n", nombres_disponibles);
        free(nombres_disponibles);
    } else {
        printf("No hay Pokémon disponibles.\n");
    }

    // Imprimir pistas
    printf("Pista del jugador:\n");
    imprimir_pista(tp->pista->pista_jugador);

    printf("Pista del bot:\n");
    imprimir_pista(tp->pista->pista_bot);

    abb_destruir_todo(tp->abb_pokemon, destruir_pokemon);
    lista_destruir_todo(tp->pista->pista_jugador, free);
    lista_destruir_todo(tp->pista->pista_bot, free);
    free(tp->pista);
    free(tp);
}

void probar_seleccionar_pokemon(const char *nombre_archivo, const char *nombre_pokemon_jugador) {
    TP *tp = tp_crear(nombre_archivo);
    if (!tp) {
        fprintf(stderr, "Error al crear el TP\n");
        return;
    }

    if (tp_seleccionar_pokemon(tp, JUGADOR_1, nombre_pokemon_jugador)) {
        const struct pokemon_info *pokemon = tp_pokemon_seleccionado(tp, JUGADOR_1);
        printf("Pokémon seleccionado para el jugador: %s\n", pokemon->nombre);
        printf("Pokémon seleccionado para la computadora: %s\n", tp->seleccionado_jugador[COMPUTADORA]->nombre);
        if (tp->seleccionado_jugador[JUGADOR_1] == tp->seleccionado_jugador[COMPUTADORA]) {
            printf("Error: El jugador y la computadora tienen el mismo Pokémon seleccionado.\n");
        }
    } else {
        printf("Error al seleccionar Pokémon\n");
    }

    // Imprimir pistas
    printf("Pista del jugador:\n");
    imprimir_pista(tp->pista->pista_jugador);

    printf("Pista del bot:\n");
    imprimir_pista(tp->pista->pista_bot);

    abb_destruir_todo(tp->abb_pokemon, destruir_pokemon);
    lista_destruir_todo(tp->pista->pista_jugador, free);
    lista_destruir_todo(tp->pista->pista_bot, free);
    free(tp->pista);
    free(tp);
}

int main() {
    const char *nombre_archivo = "pokemones.txt";
    const char *nombre_pokemon_buscar = "Pikachu";

    // Pruebas de carga y búsqueda
    probar_carga_y_busqueda_abb(nombre_archivo, nombre_pokemon_buscar);

    // Pruebas de selección de Pokémon
    probar_seleccionar_pokemon(nombre_archivo, "Pikachu");

    return 0;
}