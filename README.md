## Introduccion de problemas durante la resolución 

1- Representación de la pista: Inicialmente, asumí incorrectamente que en la pista debían incluirse caracteres como "-" para representar segmentos vacíos. Más tarde me di cuenta de que la pista debería contener únicamente los obstáculos y no los caracteres "-", lo cual me llevó a ajustar mi enfoque para manejar adecuadamente los elementos correctos que pueden estar en una pista de carreras.

2- Uso inicial de strings en lugar de listas: Comencé utilizando strings para representar las pistas en lugar de listas. Sin embargo, posteriormente comprendí (gracias a un comentario de Lucas) que las listas serían más apropiadas porque permiten manejar de manera más flexible la inserción, eliminación y ordenación de los obstáculos y otros elementos de la pista.

3- Interpretación incorrecta de tp_obstaculo_pista: En un momento dado, malinterpreté la función tp_obstaculo_pista. Pensé erróneamente que debía devolver un string ordenado mediante un sort, en lugar de mantener el orden original de inserción de los obstáculos. Esta confusión afectó mi implementación inicial de la función y me llevó a revisar y corregir el enfoque.

4- Limitaciones de tiempo para mejorar el interfaz: Me habría gustado poder mejorar más el resultado final del trabajo, pero debido a restricciones de tiempo, no pude implementar todas las funcionalidades o mejoras adicionales que tenía en mente.

5- Falta de planificación inicial: Un problema significativo fue mi enfoque inicial de programar sin realizar una planificación de las estructuras de datos necesarias. Esta falta de análisis previo me llevó a trabarme más de lo esperado ya que tenia que ir haciendo ajustes sobre la marcha y tener que volver a modificar funciones una y otra vez como tp_crear().

## Estructura Interna del TDA TP
El TDA TP está compuesto por los siguientes elementos:

abb_t *abb_pokemon: Un árbol binario de búsqueda (ABB) que almacena los pokémon disponibles. Use esta estructura porque al insertarlos de forma ordenada despues podia obtener los pokemones ordenados con el recorrido INORDEN, ademas es una buena estructura para almacenar datos y la busqueda es log(n) en comparacion con otras estructuras.

struct pokemon_info *seleccionado_jugador[2]: Un arreglo de longitud 2 para almacenar los pokémon seleccionados por cada jugador. La posición 0 corresponde al jugador 1 y la posición 1 al jugador 2.

pista_carreras_t *pista: Una estructura que contiene información sobre las pistas de carreras para ambos jugadores, incluyendo la longitud de las pistas. pense en usar estra estructura ya que en el TDA lista tengo las funciones de insertar/eliminar en una posicion especifica.

int dificultad: Una forma de guardarme la dificultad ingresada por el usuario para usarlo al momento de crear las pistas e inicializar las cantidades de obstaculos/vidas
int vidas: guarda las cantidades de veces que el jugador puede repetir la carrera

## Complejidades

* tp_crear(const char *nombre_archivo):

  Complejidad Temporal: O(n log n)
  - Inserción de cada Pokémon en el árbol binario de búsqueda (ABB) tiene complejidad de O(log n).
  - Se lee y procesa cada línea del archivo una vez, por lo tanto, O(n) en total para n Pokémon.

* tp_agregar_obstaculo(TP *tp, enum TP_JUGADOR jugador, enum TP_OBSTACULO obstaculo, unsigned posicion):

  Complejidad Temporal: O(m)
  - n es el tamaño de la lista en la pista del jugador o del bot. Operaciones como la inserción en una posición específica en una lista tienen complejidad O(n).

* tp_quitar_obstaculo(TP *tp, enum TP_JUGADOR jugador, unsigned posicion):

  Complejidad Temporal: O(n)
  - Por lo mismo de la inserción, n es el tamaño de la lista en la pista del jugador o del bot.

* tp_limpiar_pista(TP *tp, enum TP_JUGADOR jugador):

  Complejidad Temporal: O(n)
  - Para poder eliminar cada nodo de la lista hay que iterar elemento a elemento.

* tp_calcular_tiempo_pista(TP *tp, enum TP_JUGADOR jugador):

  Complejidad Temporal: O(n)
  - Recorre cada obstáculo en la pista del jugador o del bot una vez, realizando operaciones constantes O(1) por obstáculo.

* tp_tiempo_por_obstaculo(TP *tp, enum TP_JUGADOR jugador):

  Complejidad Temporal: O(n)
  - Al igual que en tp_calcular_tiempo_pista, recorre cada obstáculo en la pista del jugador o del bot una vez, realizando operaciones constantes O(1) por obstáculo.

* tp_destruir(TP *tp):

  Complejidad Temporal: O(n + m)
  - n es el número de Pokémon en el árbol y m es el tamaño de las listas de obstáculos.
  - Libera todos los recursos asignados dinámicamente, incluyendo los Pokémon en el ABB y los obstáculos en las listas.

### Funciones Generales

1. **void limpiar_pantalla();**
   - Limpia la pantalla de la consola.

2. **void esperar_ingreso_k();**
   - Espera la entrada de la tecla 'K' para continuar.

### Funciones de Lista y Pista

1. **bool imprimir_elemento(void *elemento, void *contexto);**
   - Imprime un elemento de la lista (pista).

2. **void pista_imprimir(lista_t *lista);**
   - Imprime los elementos de una lista que representa la pista.

3. **lista_t *pista_crear_e_inicializar(int longitud);**
   - Crea una lista de la longitud especificada y la inicializa con el carácter '-'.

### Funciones de Obstáculos

1. **void insertar_obstaculos_pista_bot(TP *tp);**
   - Inserta obstáculos en la pista del bot según la dificultad del juego.

### Funciones de Inicialización

1. **TP *inicializar_juego(TP *tp, int dificultad);**
   - Inicializa el juego con la dificultad especificada.

### Funciones de Selección de Pokémon

1. **bool seleccionar_pokemon_en_iteracion(void *elemento, void *contexto);**
   - Selecciona un Pokémon específico durante una iteración.

2. **const struct pokemon_info* seleccionar_pokemon_aleatorio(TP *tp);**
   - Selecciona un Pokémon aleatorio de la lista de Pokémon disponibles.

3. **bool inicializar_seleccion_pokemon_bot(TP *tp);**
   - Inicializa la selección de Pokémon para el bot.

### Funciones del Menú

1. **menu_t* menu_crear();**
   - Crea un menú vacío.

2. **void mostrar_ayuda(void* menu);**
   - Muestra la ayuda con los comandos disponibles.

3. **void mostrar_pokemon(void* menu);**
   - Muestra los Pokémon disponibles para la carrera.

4. **bool iniciar_juego(void* menu);**
   - Inicia el juego y muestra las opciones de menú.

### Funciones de Simulación y Cálculo

1. **void simular_pokemon(lista_t *lista, unsigned tiempo_total, char jugador);**
   - Simula el avance de un Pokémon en la pista.

2. **unsigned calcular_puntaje(unsigned Ta, unsigned Tb);**
   - Calcula el puntaje final de la carrera.

3. **void imprimir_tiempos(unsigned tiempo_jugador1, unsigned tiempo_jugador2, unsigned puntaje);**
   - Imprime los tiempos y el puntaje final de la carrera.

### Funciones de Impresión

1. **void imprimir_pokemon(const char *nombre);**
   - Imprime la representación de un Pokémon específico.

2. **void explicar_obstaculos();**
   - Explica los tipos de obstáculos en la pista.

3. **void imprimir_opciones();**
   - Imprime las opciones disponibles para el jugador.

### Funciones de Gestión de Menú

1. **bool procesar_comando(menu_t* menu, const char* comando);**
   - Procesa un comando del menú.

2. **void agregar_opcion_menu(menu_t* menu, const char* comando, void (*funcion_a_ejecutar)(void*), const char* descripcion);**
   - Agrega una opción al menú.

3. **void inicializar_menu(menu_t* menu);**
   - Inicializa el menú con las opciones predefinidas.

4. **void liberar_menu(menu_t* menu);**
   - Libera la memoria utilizada por el menú.

## Grafico de memoria 
<div style="text-align: center;">
  <img width="1000px" src="img/Memoria.PNG">
</div>

