#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prints.h"
#include "tp.h"

#define LONGITUD_MAX_COMANDO 10

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

typedef struct {
    const char *nombre;
    void (*funcion)();
} nombre_funcion_t;


typedef struct lista lista_t;
typedef struct abb abb_t;

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
}TP;

// Array de estructuras nombre_funcion_t para mapear nombres de Pokémon a funciones de impresión
nombre_funcion_t funciones_pokemon[] = {
    {"Pikachu", print_pikachu},
    {"Dragonair", print_dragonair},
    {"Caterpie", print_caterpie},
    {"Digglet", print_digglet},
    {"Dragonite", print_dragonite},
    {"Ekkans", print_ekkans},
    {"Geodude", print_geodude},
    {"Grimer", print_grimer},
    {"Meowth", print_meowth},
    {"Pidgey", print_pidgey},
    {"Ratata", print_ratata},
    {"Moltres", print_moltres},
    {"Zapdos", print_zapdos},
    {"Articuno", print_articuno},
    {"Vulpix", print_vulpix},
    {"Sandshrew", print_sandshrew},
    {"Raichu", print_raichu},
    {"Venusaur", print_venusaur},
    {"Bulbasaur", print_bulbasaur},
    {"Charmander", print_charmander},
    {"Squirtle", print_squirtle},
    {"Wartortle", print_wartortle},
    {"Blastoise", print_blastoise},
    {NULL, NULL} // Marca de fin del array
};


menu_t* menu_crear() {
    menu_t* menu = calloc(1, sizeof(menu_t));
    if (menu == NULL) {
        perror("No se pudo crear el menú");
        return NULL;
    }

    return menu;
}

void mostrar_ayuda(void* menu) {
    menu_t* m = (menu_t*)menu;
    printf("Comandos disponibles:\n");
    for (size_t i = 0; i < m->cantidad_opciones; i++) {
        printf("  %s - %s\n", m->opciones[i].comando, m->opciones[i].descripcion);
    }
}

// Función para imprimir un Pokémon específico si existe una función asociada
void imprimir_pokemon(const char *nombre) {
    for (int i = 0; funciones_pokemon[i].nombre != NULL; i++) {
        if (strcmp(funciones_pokemon[i].nombre, nombre) == 0) {
            printf("%s\n", nombre);
            funciones_pokemon[i].funcion();
            return;
        }
    }

    // Si no se encontró el nombre en el array imprimir el nombre solo
    printf("%s\n", nombre);
}

void mostrar_pokemon(void* menu) {
    menu_t* m = (menu_t*)menu;
    printf("Pokemons disponibles para la carrera:\n");

    char* nombres_pokemons = tp_nombres_disponibles(m->tp);
    if (nombres_pokemons == NULL) {
        printf("No se pudieron obtener los nombres de los Pokémon.\n");
        return;
    }

    // Separar los nombres de Pokémon por coma
    char* nombre = strtok(nombres_pokemons, ",");
    while (nombre != NULL) {
        if (strcmp(nombre, "Pikachu") == 0) print_pikachu();
        else if (strcmp(nombre, "Dragonair") == 0) print_dragonair();
        else if (strcmp(nombre, "Caterpie") == 0) print_caterpie();
        else if (strcmp(nombre, "Digglet") == 0) print_digglet();
        else if (strcmp(nombre, "Dragonite") == 0) print_dragonite();
        else if (strcmp(nombre, "Ekkans") == 0) print_ekkans();
        else if (strcmp(nombre, "Geodude") == 0) print_geodude();
        else if (strcmp(nombre, "Grimer") == 0) print_grimer();
        else if (strcmp(nombre, "Meowth") == 0) print_meowth();
        else if (strcmp(nombre, "Pidgey") == 0) print_pidgey();
        else if (strcmp(nombre, "Ratata") == 0) print_ratata();
        else if (strcmp(nombre, "Moltres") == 0) print_moltres();
        else if (strcmp(nombre, "Zapdos") == 0) print_zapdos();
        else if (strcmp(nombre, "Articuno") == 0) print_articuno();
        else if (strcmp(nombre, "Vulpix") == 0) print_vulpix();
        else if (strcmp(nombre, "Sandshrew") == 0) print_sandshrew();
        else if (strcmp(nombre, "Raichu") == 0) print_raichu();
        else if (strcmp(nombre, "Venusaur") == 0) print_venusaur();
        else if (strcmp(nombre, "Bulbasaur") == 0) print_bulbasaur();
        else if (strcmp(nombre, "Charmander") == 0) print_charmander();
        else if (strcmp(nombre, "Squirtle") == 0) print_squirtle();
        else if (strcmp(nombre, "Wartortle") == 0) print_wartortle();
        else if (strcmp(nombre, "Blastoise") == 0) print_blastoise();
        else {
            printf("%s\n", nombre);
        }

        nombre = strtok(NULL, ",");
    }

    free(nombres_pokemons);
}

void iniciar_juego(void* menu) {
    menu_t* m = (menu_t*)menu;
    printf("Iniciando el juego...\n");

    // Crear la estructura TP
    m->tp = tp_crear("nombre_archivo");
    if (m->tp == NULL) {
        printf("Error al crear la estructura TP.\n");
        return;
    }

    // Mostrar el menú inicial
    print_menu();

    // Selección de dificultad
    int dificultad;
    printf("Seleccione la dificultad (1-3): ");
    scanf("%d", &dificultad);

    // Inicializar juego con la dificultad seleccionada
    inicializar_juego(m->tp, dificultad);

    // Inicialización de la selección de Pokémon para la computadora
    if (!inicializar_seleccion_pokemon(m->tp)) {
        printf("Error al seleccionar el Pokémon para la computadora.\n");
        return;
    }

    // Mostrar el Pokémon seleccionado para la computadora
    printf("La computadora ha seleccionado a:\n");
    imprimir_pokemon(m->tp->seleccionado_jugador[JUGADOR_2]->nombre);

    printf("La pista del bot es:\n");
    pista_imprimir(m->tp->pista->pista_bot);

    // Selección de Pokémon por parte del jugador
    printf("Seleccione su Pokémon");
    char nombre_pokemon[100];
    scanf("%99s", nombre_pokemon);

    if (!tp_seleccionar_pokemon(m->tp, JUGADOR_1, nombre_pokemon)) {
        printf("Error al seleccionar el Pokémon para el jugador.\n");
        return;
    }

    printf("Usted ha seleccionado a:\n");
    imprimir_pokemon(nombre_pokemon);

    // Selección del largo de la pista
    printf("Seleccione el largo de su pista, su longitud no tiene límite: ");
    size_t largo;
    scanf("%zu", &largo);

    // Asignar el largo de la pista al juego
    m->tp->pista->longitud_pista_jugador = largo;
    m->tp->pista->pista_jugador = pista_crear_e_inicializar(m->tp->pista->longitud_pista_jugador);

    pista_imprimir(m->tp->pista->pista_bot);
    pista_imprimir(m->tp->pista->pista_jugador);

    printf("Ahora ingrese los obstáculos para su Pokémon:\n");
    // Aquí debería implementarse la inserción de obstáculos por parte del jugador

}

void mostrar_creditos(void* menu){
    printf("Este juego fue programado por:\n");
    printf("Scopel Nicolas Ignacio :)\n");
}

void procesar_comando(menu_t* menu, const char* comando) {
    for (size_t i = 0; i < menu->cantidad_opciones; i++) {
        if (strcmp(menu->opciones[i].comando, comando) == 0) {
            menu->opciones[i].funcion_a_ejecutar(menu);
            return;
        }
    }
    printf("Comando no reconocido. Escriba 'h' para ayuda.\n");
}

void agregar_opcion_menu(menu_t* menu, const char* comando, void (*funcion_a_ejecutar)(void*), const char* descripcion) {
    menu->opciones = realloc(menu->opciones, sizeof(opcion_menu_t) * (menu->cantidad_opciones + 1));
    if (menu->opciones == NULL) {
        perror("No se pudo agregar la opción del menú");
        return;
    }
    strncpy(menu->opciones[menu->cantidad_opciones].comando, comando, LONGITUD_MAX_COMANDO - 1);
    menu->opciones[menu->cantidad_opciones].funcion_a_ejecutar = funcion_a_ejecutar;
    strncpy(menu->opciones[menu->cantidad_opciones].descripcion, descripcion, sizeof(menu->opciones[menu->cantidad_opciones].descripcion) - 1);
    menu->cantidad_opciones++;
}

void inicializar_menu(menu_t* menu) {
    agregar_opcion_menu(menu, "h", mostrar_ayuda, "Muestra este mensaje de ayuda");
    agregar_opcion_menu(menu, "p", mostrar_pokemon, "Muestra los Pokemon disponibles");
    agregar_opcion_menu(menu, "s", iniciar_juego, "Empieza el juego");
    agregar_opcion_menu(menu, "c", mostrar_creditos, "Creditos del juego");
}

void liberar_menu(menu_t* menu) {
    if (menu == NULL) return;
    free(menu->opciones);
    if (menu->tp != NULL) {
        tp_destruir(menu->tp);
    }
    free(menu);
}

int main() {
    menu_t* menu = menu_crear();
    if (menu == NULL) {
        return EXIT_FAILURE;
    }
    inicializar_menu(menu);

    char comando[LONGITUD_MAX_COMANDO];
    while (1) {
        printf("Ingrese un comando (h para ayuda): ");
        scanf("%9s", comando);
        procesar_comando(menu, comando);
    }

    liberar_menu(menu);
    return 0;
}