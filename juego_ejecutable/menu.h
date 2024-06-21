#ifndef MENU_H
#define MENU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prints.h"
#include "tp.h"
#include "lista.h"
#include "abb.h"
#include <time.h>
#include <unistd.h>

#define LONGITUD_MAX_COMANDO 10
#define LONGITUD_BASE 50
#define OBSTACULOS_FACIL 3
#define OBSTACULOS_MEDIO 5
#define OBSTACULOS_DIFICIL 7
#define OBSTACULOS_IMPOSIBLE 10

typedef struct {
    char comando[LONGITUD_MAX_COMANDO];
    void (*funcion_a_ejecutar)(void*);
    char descripcion[100];
} opcion_menu_t;

typedef struct {
    opcion_menu_t* opciones;
    size_t cantidad_opciones;
    TP* tp;
} menu_t;

typedef struct nodo {
    void *elemento;
    struct nodo *siguiente;
} nodo_t;

typedef struct lista lista_t;

typedef struct {
    const char *nombre;
    void (*funcion)();
} nombre_funcion_t;

typedef struct {
    int longitud_pista_bot;
    int longitud_pista_jugador;
    lista_t *pista_jugador;
    lista_t *pista_bot;
    lista_t *obstaculos_jugador;
    lista_t *obstaculos_bot;
} pista_carreras_t;

typedef struct tp {
    abb_t *abb_pokemon;
    struct pokemon_info *seleccionado_jugador[2]; // primera pos es siempre el jugador, segunda pos es siempre el bot
    pista_carreras_t *pista;
    int dificultad; // 0: Facil, 1: Medio, 2:Dificil, 3:Imposible 
    int vidas;
} TP;

void limpiar_pantalla();
bool imprimir_elemento(void *elemento, void *contexto);
void pista_imprimir(lista_t *lista);
lista_t *pista_crear_e_inicializar(int longitud);
void insertar_obstaculos_pista_bot(TP *tp);
TP *inicializar_juego(TP *tp, int dificultad);
bool seleccionar_pokemon_en_iteracion(void *elemento, void *contexto);
const struct pokemon_info* seleccionar_pokemon_aleatorio(TP *tp);
bool inicializar_seleccion_pokemon_bot(TP *tp);
menu_t* menu_crear();
void mostrar_ayuda(void* menu);
void imprimir_pokemon(const char *nombre);
void simular_pokemon(lista_t *lista, unsigned tiempo_total, char jugador);
void mostrar_pokemon(void* menu);
unsigned calcular_puntaje(unsigned Ta, unsigned Tb);
void esperar_ingreso_k();
void imprimir_tiempos(unsigned tiempo_jugador1, unsigned tiempo_jugador2, unsigned puntaje);
void explicar_obstaculos();
void imprimir_opciones();
void iniciar_juego(void* menu);
void mostrar_creditos(void* menu);
void mostrar_tutorial(void* menu);
void procesar_comando(menu_t* menu, const char* comando);
void agregar_opcion_menu(menu_t* menu, const char* comando, void (*funcion_a_ejecutar)(void*), const char* descripcion);
void inicializar_menu(menu_t* menu);
void liberar_menu(menu_t* menu);

#endif /* MENU_H */