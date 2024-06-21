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

typedef struct {
    const char *nombre;
    void (*funcion)();
} nombre_funcion_t;


typedef struct nodo {
    void *elemento;
    struct nodo *siguiente;
} nodo_t;

struct lista {
    nodo_t *nodo_inicio;
    size_t longitud;
    nodo_t *nodo_final;
};

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
    int vidas;
}TP;

// Estructura para el contexto de iteración
struct datos_de_iteracion {
    size_t iteracion_actual;
    size_t iteracion_objetivo;
    const struct pokemon_info **pokemon_seleccionado;
};

// Array de estructuras nombre_funcion_t para mapear nombres de Pokémon a funciones de impresión
nombre_funcion_t funciones_pokemon[] = {
    {"Pikachu", print_pikachu},
    {"Dragonair", print_dragonair},
    {"Caterpie", print_caterpie},
    {"Digglett", print_digglett},
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

void limpiar_pantalla() {
    printf("\033[H\033[J");
}
// Funciones para el manejo correcto del juego
bool imprimir_elemento(void *elemento, void *contexto) {
    char *elem = (char *)elemento;
    printf("%c", *elem);  // Imprimir el carácter
    return true;
}

void pista_imprimir(lista_t *lista) {
    if (lista == NULL) {
        printf("Lista no válida.\n");
        return;
    }

    size_t elementos_iterados = lista_con_cada_elemento(lista, imprimir_elemento, NULL);

    if (elementos_iterados == 0) {
        printf("Error al iterar sobre la pista o la pista está vacía.\n");
    } else {
        printf("\n");  // Añadir un salto de línea al final de la impresión
    }
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

    int longitud_pista = tp->pista->longitud_pista_bot;

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
        tp_agregar_obstaculo(tp, JUGADOR_2, tipo_obstaculo, posicion);
    }
}

TP *inicializar_juego(TP *tp, int dificultad) {
    if (tp == NULL)
        return NULL;

    // Inicializar la dificultad
    tp->dificultad = dificultad;

    switch (dificultad) {
        case 0:
            tp->vidas = 3;
            break;
        case 1:
            tp->vidas = 2;
            break;
        case 2:
            tp->vidas = 1;
            break;
        default:
            tp->vidas = 3;
            break;
    }
    // Inicializar longitud de la pista del bot según la dificultad
    tp->pista->longitud_pista_bot = LONGITUD_BASE + (dificultad * 25);

    // Crear las pista vacía/Inicializa las pista con el caracter "-"
    tp->pista->pista_bot = pista_crear_e_inicializar(tp->pista->longitud_pista_bot);
    tp->pista->pista_jugador = lista_crear();

    // Verificación de que no haya fallado ningún calloc
    if (!tp->pista->pista_jugador || !tp->pista->pista_bot) {
        fprintf(stderr, "No se pudo crear las pistas\n");
        lista_destruir(tp->pista->pista_bot);
        lista_destruir(tp->pista->pista_jugador);
        return NULL;
    }

    //Inserta obstaculos de forma aleatoria en la pista del bot
    insertar_obstaculos_pista_bot(tp);

    return tp;
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

const struct pokemon_info* seleccionar_pokemon_aleatorio(TP *tp) {
    if (tp == NULL || tp->abb_pokemon == NULL) {
        return NULL;
    }

    size_t cantidad_pokemones = (size_t)tp_cantidad_pokemon(tp);
    srand((unsigned int)time(NULL));
    size_t iteracion_objetivo = (size_t)(rand() % (int)cantidad_pokemones);
    const struct pokemon_info *pokemon_seleccionado = NULL;

    struct datos_de_iteracion datos = {0, iteracion_objetivo, &pokemon_seleccionado};
    abb_con_cada_elemento(tp->abb_pokemon, INORDEN, seleccionar_pokemon_en_iteracion, &datos);

    return pokemon_seleccionado;
}

bool inicializar_seleccion_pokemon_bot(TP *tp) {
    if (tp == NULL) {
        return false;
    }

    const struct pokemon_info *pokemon_JUGADOR_2 = seleccionar_pokemon_aleatorio(tp);

    if (pokemon_JUGADOR_2 == NULL) {
        return false;
    }

    tp->seleccionado_jugador[JUGADOR_2] = (struct pokemon_info *)pokemon_JUGADOR_2;
    return true;
}


//Funciones sobre el menu

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
            printf("        %s\n", nombre);
            funciones_pokemon[i].funcion();
            return;
        }
    }

    // Si no se encontró el nombre en el array imprimir el nombre solo
    printf("       %s\n", nombre);
}

// Función para simular el avance de un Pokémon en la pista
void simular_pokemon(lista_t *lista, unsigned tiempo_total, char jugador) {
    size_t posicion_jugador = 0;
    unsigned pasos_totales = lista->longitud;
    unsigned sleep_time = (tiempo_total * 1000000) / pasos_totales;

    // Variable para guardar el carácter anterior
    void *caracter_anterior = lista_elemento_en_posicion(lista, posicion_jugador);

    while (posicion_jugador < lista->longitud) {
        system("clear");

        if (posicion_jugador > 0) {
            cambiar_elemento(lista, posicion_jugador - 1, caracter_anterior);
        }

        caracter_anterior = lista_elemento_en_posicion(lista, posicion_jugador);

        cambiar_elemento(lista, posicion_jugador, &jugador);
        posicion_jugador++;

        pista_imprimir(lista);
        usleep(sleep_time);
    }
    if (posicion_jugador > 0) {
        cambiar_elemento(lista, posicion_jugador - 1, caracter_anterior);
    }
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
        else if (strcmp(nombre, "Digglett") == 0) print_digglett();
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
        printf("%s\n", nombre);
        nombre = strtok(NULL, ",");
    }

    free(nombres_pokemons);
}

// Función para calcular el puntaje final de la carrera
unsigned calcular_puntaje(unsigned Ta, unsigned Tb) {
    if (Ta == 0 && Tb == 0) {
        return 0;  // Evita división por cero
    }

    unsigned suma = Ta + Tb;
    unsigned puntaje = 100 - (100 * abs((int)(Ta - Tb)) / suma);

    return puntaje;
}

// Implementación de la función para esperar la entrada 'K' o 'k'
void esperar_ingreso_k() {
    char input;
    do {
        input = getchar();
        // Limpiar el buffer de entrada
        while (getchar() != '\n');
    } while (input != 'K' && input != 'k');
}
void imprimir_tiempos(unsigned tiempo_jugador1, unsigned tiempo_jugador2, unsigned puntaje){
    printf("Tiempo del corredor 1: %u segundos\n", tiempo_jugador1);
    printf("Tiempo del corredor 2: %u segundos\n", tiempo_jugador2);
    printf("El puntaje final de la carrera es: %u\n", puntaje);
}

void explicar_obstaculos(){
    printf("- Si ves '%c' en la pista, es un obstáculo de fuerza, es piedra enorme que requiere fuerza física para moverla.\n", IDENTIFICADOR_OBSTACULO_FUERZA);
    printf("- Si ves '%c' en la pista, es un obstáculo de destreza, es un camino estrecho que requiere habilidad para ser navegado.\n", IDENTIFICADOR_OBSTACULO_DESTREZA);
    printf("- Si ves '%c' en la pista, es un obstáculo de inteligencia, es un acertijo o rompecabezas que requiere pensamiento para ser resuelto.\n", IDENTIFICADOR_OBSTACULO_INTELIGENCIA);
}

void imprimir_opciones(){
    printf("F para insertar un obstaculo de fuerza\n");
    printf("D para insertar un obstaculo de destreza\n");
    printf("I para insertar un obstaculo de inteligencia\n");
    printf("E para eleminar un obstaculo\n");
    printf("K para continuar...\n");
}

void iniciar_juego(void* menu) {
    menu_t* m = (menu_t*)menu;
    printf("Iniciando el juego...\n");

    // Crear la estructura TP
    m->tp = tp_crear("pokemones.txt");
    if (m->tp == NULL) {
        printf("Error al crear la estructura TP.\n");
        return;
    }

    int repetir = 1; // Variable para controlar si se repite el juego

    do {
        // Mostrar el menú inicial
        print_menu();

        // Selección de dificultad
        int dificultad;
        printf("Seleccione la dificultad (0-3): ");
        scanf(" %d", &dificultad);

        // Inicializar juego con la dificultad seleccionada
        inicializar_juego(m->tp, dificultad);

        // Inicialización de la selección de Pokémon para la computadora
        if (!inicializar_seleccion_pokemon_bot(m->tp)) {
            printf("Error al seleccionar el Pokémon para la computadora.\n");
            return;
        }

        system("clear");

        // Mostrar el Pokémon seleccionado para la computadora
        printf("Tendrás que correr contra...\n");
        imprimir_pokemon(m->tp->seleccionado_jugador[JUGADOR_2]->nombre);
        printf("Ingresa K para continuar\n");
        esperar_ingreso_k();

        system("clear");

        printf("La pista de tu contrincante:\n");
        pista_imprimir(m->tp->pista->pista_bot);
        printf("- Posee una longitud de: %zu\n", lista_tamanio(m->tp->pista->pista_bot));
        printf("- La cantidad de obstáculos en la pista es de: %zu\n", lista_tamanio(m->tp->pista->obstaculos_bot));
        explicar_obstaculos();
        printf("Ingresa K para continuar\n");
        esperar_ingreso_k();
        system("clear");

        // Selección de Pokémon por parte del jugador
        mostrar_pokemon(m);
        printf("Seleccione su Pokémon: \n");
        char nombre_pokemon[100];
        scanf("%99s", nombre_pokemon);

        if (!tp_seleccionar_pokemon(m->tp, JUGADOR_1, nombre_pokemon)) {
            printf("Error al seleccionar el Pokémon para el jugador.\n");
            return;
        }

        system("clear");
        printf("Usted ha seleccionado a:\n");
        imprimir_pokemon(nombre_pokemon);

        // Selección del largo de la pista
        printf("Seleccione el largo de su pista: \n");
        int largo;
        scanf("%i", &largo);
        system("clear");
        // Asignar el largo de la pista al juego
        m->tp->pista->longitud_pista_jugador = largo;
        m->tp->pista->pista_jugador = pista_crear_e_inicializar(m->tp->pista->longitud_pista_jugador);
        printf("Pista bot:\n");
        pista_imprimir(m->tp->pista->pista_bot);
        printf("Su pista:\n");
        pista_imprimir(m->tp->pista->pista_jugador);

        char opcion;
        unsigned int posicion;
        size_t pista_jugador_tamanio = lista_tamanio(m->tp->pista->pista_jugador);
        int vida_restante = m->tp->vidas; // Guardar las vidas restantes antes de la carrera

        printf("Ahora ingrese los obstáculos para su pista\n");

        do {
            imprimir_opciones();
            scanf(" %c", &opcion);

            // Verificar que la opción ingresada sea válida
            while (opcion != 'K' && opcion != 'F' && opcion != 'D' && opcion != 'I' && opcion != 'E') {
                printf("Opción no válida. Intente de nuevo.\n");
                imprimir_opciones();
                scanf(" %c", &opcion);
            }

            if (opcion != 'K') {
                if (opcion == 'E') {
                    // Eliminar obstáculo
                    system("clear");
                    printf("Ingrese la posición del obstáculo que desea eliminar: ");
                    scanf("%u", &posicion);

                    // Validar que la posición esté dentro de los límites válidos
                    while (posicion >= pista_jugador_tamanio) {
                        printf("Posición no válida. Intente de nuevo.\n");
                        printf("Ingrese la posición del obstáculo que desea eliminar: ");
                        scanf("%u", &posicion);
                    }

                    // Llamar a la función para eliminar el obstáculo
                    tp_quitar_obstaculo(m->tp, JUGADOR_1, posicion);
                    pista_imprimir(m->tp->pista->pista_jugador);
                } else {
                    // Agregar obstáculo
                    int tipo = 0;
                    switch (opcion) {
                        case 'F':
                            tipo = 0;
                            break;
                        case 'D':
                            tipo = 1;
                            break;
                        case 'I':
                            tipo = 2;
                            break;
                        default:
                            break;
                    }

                    system("clear");
                    printf("Ahora ingrese en qué parte de la pista lo quiere ingresar\n");
                    printf("Los valores válidos son desde 0 hasta %zu\n", pista_jugador_tamanio - 1);
                    scanf("%u", &posicion);

                    // Validar que la posición esté dentro de los límites válidos
                    while (posicion >= pista_jugador_tamanio) {
                        printf("Posición no válida. Intente de nuevo.\n");
                        printf("Los valores válidos son desde 0 hasta %zu\n", pista_jugador_tamanio - 1);
                        scanf("%u", &posicion);
                    }

                    system("clear");
                    printf("La cantidad total de obstáculos en su pista es %u\n", tp_agregar_obstaculo(m->tp, 0, tipo, posicion));
                    pista_imprimir(m->tp->pista->pista_jugador);
                }
            }
        } while (opcion != 'K');

        printf("La carrera va a Empezar en...\n");
        sleep(2);

        system("clear");
        print_tres();
        sleep(2);

        system("clear");
        print_dos();
        sleep(2);

        system("clear");
        print_uno();
        sleep(3);

        system("clear");
        print_go();
        sleep(1);
        system("clear");

        unsigned tiempo_jugador1 = tp_calcular_tiempo_pista(m->tp, JUGADOR_1);
        unsigned tiempo_jugador2 = tp_calcular_tiempo_pista(m->tp, JUGADOR_2);
        printf("Tiempo jugador 1 = %u\n", tiempo_jugador1);
        printf("Tiempo jugador 2 = %u\n", tiempo_jugador2);

        printf("Carrera del jugador 2: \n");
        simular_pokemon(m->tp->pista->pista_bot, tiempo_jugador2, '2');
        printf("Tiempo jugador 2 = %u\n", tiempo_jugador2);

        printf("Ingresa K para continuar\n");
        esperar_ingreso_k();
        system("clear");

        printf("Carrera del jugador 1: \n");
        simular_pokemon(m->tp->pista->pista_jugador, tiempo_jugador1, '1');
        printf("Tiempo jugador 1 = %u\n", tiempo_jugador1);

        printf("Ingresa K para continuar\n");
        esperar_ingreso_k();
        system("clear");

        unsigned puntaje = calcular_puntaje(tiempo_jugador2, tiempo_jugador1);
        imprimir_tiempos(tiempo_jugador1, tiempo_jugador2, puntaje);

        // Restar vida después de la carrera
        m->tp->vidas--;

        // Verificar si el jugador quiere repetir la carrera
        if (m->tp->vidas > 0) {
            printf("¿Desea intentarlo de nuevo? Le quedan %i vidas\n", m->tp->vidas);
            printf("Ingrese S (SI) o N (NO)\n");
            char respuesta;
            scanf(" %c", &respuesta);

            if (respuesta != 'S' && respuesta != 's') {
                repetir = 0; // Salir del bucle de repetición
            }
        } else {
            printf("No le quedan más vidas. Juego terminado.\n");
            repetir = 0; // Salir del bucle de repetición
        }

    } while (repetir);

    printf("Gracias por jugar!\n");
}

void mostrar_creditos(void* menu){
    system("clear");
    printf("Este juego fue programado por:\n");
    printf("Scopel Nicolas Ignacio :)\n");
}
void mostrar_tutorial(void* menu){
    system("clear");
    printf("\nBienvenido al juego TP: Carrera de obstáculos Pokemon!\n");
    printf("En este juego, competirás contra otro Pokemon en una carrera de obstáculos.\n");
    printf("Aquí están las reglas y cómo funciona:\n");
    printf("1. Al iniciar, seleccionarás la dificultad del juego (fácil, normal, difícil, imposible).\n");
    printf("2. Se seleccionará al azar un Pokemon para competir y se creará una pista de obstáculos.\n");
    printf("3. La longitud de la pista generada depende de la dificultad seleccionada.\n");
    printf("4. Se te mostrará la información del Pokemon contra el que competirás y detalles de la pista.\n");
    printf("5. Se omitirá información acerca de algunos obstáculos, dependiendo de la dificultad.\n");
    printf("6. Se te pedirá que selecciones tu propio Pokemon y armes tu pista de obstáculos.\n");
    printf("7. Después de armar tu pista, se correrá una carrera entre ambos Pokemon.\n");
    printf("8. El objetivo es que ambos terminen la carrera al mismo tiempo o lo más cercano posible.\n");
    printf("9. Puedes modificar tu pista y reintentar para mejorar tu puntaje, según la dificultad.\n");
    printf("10. El puntaje se calcula en base a la cercanía de los tiempos de ambos Pokemon.\n");
    printf("\n¡Diviértete y buena suerte!\n\n");
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
    agregar_opcion_menu(menu, "p", mostrar_tutorial, "Tutorial sobre como jugar!");
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