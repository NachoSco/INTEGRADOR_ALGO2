#include "lista.h"
#include <stddef.h>
#include <stdlib.h>

typedef struct nodo {
	void *elemento;
	struct nodo *siguiente;
} nodo_t;

struct lista {
	nodo_t *nodo_inicio;
	size_t longitud;
	nodo_t *nodo_final;
};

struct lista_iterador {
	nodo_t *actual;
};

nodo_t *crear_nodo(void *elemento)
{
	nodo_t *Nodo_nuevo = malloc(sizeof(nodo_t));
	if (Nodo_nuevo == NULL) {
		return NULL;
	}
	Nodo_nuevo->elemento = elemento;
	Nodo_nuevo->siguiente = NULL;

	return Nodo_nuevo;
}

lista_t *lista_crear()
{
	lista_t *lista = malloc(sizeof(lista_t));
	if (lista == NULL) {
		return NULL;
	}
	lista->nodo_inicio = NULL;
	lista->longitud = 0;
	lista->nodo_final = NULL;

	return lista;
}

lista_t *lista_insertar(lista_t *lista, void *elemento)
{
	if (lista == NULL) {
		return NULL; // La lista no existe o el elemento es NULL, devuelve NULL
	}

	nodo_t *nodo_nuevo = crear_nodo(elemento);

	if (nodo_nuevo == NULL) {
		return NULL; // Error al crear el nuevo nodo, devuelve NULL
	}

	if (lista->nodo_inicio == NULL) {
		lista->nodo_inicio = nodo_nuevo;
		lista->nodo_final = nodo_nuevo;
	} else {
		lista->nodo_final->siguiente = nodo_nuevo;
		lista->nodo_final = nodo_nuevo;
	}

	lista->longitud++;
	return lista;
}

lista_t *lista_insertar_en_posicion(lista_t *lista, void *elemento,
				    size_t posicion)
{
	if (lista == NULL) {
		return NULL; // La lista no existe o el elemento es NULL, devuelve NULL
	}

	if (posicion >= lista->longitud) {
		return lista_insertar(
			lista,
			elemento); // Si la posición está más allá de la longitud actual, simplemente inserta al final
	}

	nodo_t *nodo_nuevo = crear_nodo(elemento);
	if (nodo_nuevo == NULL) {
		return NULL; // Error al crear el nuevo nodo, devuelve NULL
	}

	if (posicion == 0) {
		nodo_nuevo->siguiente = lista->nodo_inicio;
		lista->nodo_inicio = nodo_nuevo;
	} else if (posicion == 1) {
		nodo_nuevo->siguiente = lista->nodo_inicio->siguiente;
		lista->nodo_inicio->siguiente = nodo_nuevo;
	} else {
		int contador = 0;
		nodo_t *actual = lista->nodo_inicio;

		while (contador < posicion - 1) {
			actual = actual->siguiente;
			contador++;
		}
		nodo_nuevo->siguiente = actual->siguiente;
		actual->siguiente = nodo_nuevo;
	}

	lista->longitud++;
	return lista;
}

void *lista_quitar(lista_t *lista)
{
	if (lista == NULL || lista->longitud == 0) {
		return NULL; // La lista no existe o el elemento es NULL, devuelve NULL
	}

	void *elemento_a_eliminar;

	if (lista->longitud == 1) {
		elemento_a_eliminar = lista->nodo_inicio->elemento;
		free(lista->nodo_inicio);
		lista->nodo_inicio = NULL;
		lista->nodo_final = NULL;
	} else {
		nodo_t *actual = lista->nodo_inicio;
		size_t contador = 0;

		while (contador < lista->longitud - 2) {
			actual = actual->siguiente;
			contador++;
		}
		elemento_a_eliminar = lista->nodo_final->elemento;
		free(lista->nodo_final);
		actual->siguiente = NULL;
		lista->nodo_final = actual;
	}

	lista->longitud--;
	return elemento_a_eliminar;
}

void *lista_quitar_de_posicion(lista_t *lista, size_t posicion)
{
	if (lista == NULL) {
		return NULL;
	}
	if (posicion >= lista->longitud - 1) {
		return lista_quitar(lista);
	}
	if (lista->longitud == 0) {
		return NULL;
	}

	void *elemento_a_eliminar;

	if (lista->longitud == 1) {
		elemento_a_eliminar = lista->nodo_inicio->elemento;
		free(lista->nodo_inicio);
		lista->nodo_inicio = NULL;
		lista->nodo_final = NULL;

	} else {
		if (posicion == 0) {
			nodo_t *proximo_al_eliminado =
				lista->nodo_inicio->siguiente;
			elemento_a_eliminar = lista->nodo_inicio->elemento;
			free(lista->nodo_inicio);
			lista->nodo_inicio = proximo_al_eliminado;
		} else {
			nodo_t *actual = lista->nodo_inicio;
			size_t contador = 0;

			while (contador < posicion - 1) {
				actual = actual->siguiente;
				contador++;
			}
			elemento_a_eliminar = actual->siguiente->elemento;
			nodo_t *aux = actual->siguiente->siguiente;
			free(actual->siguiente);
			actual->siguiente = aux;
		}
	}

	lista->longitud--;
	return elemento_a_eliminar;
}

void *lista_elemento_en_posicion(lista_t *lista, size_t posicion)
{
	if (lista == NULL || lista->longitud == 0 ||
	    posicion >= lista->longitud) {
		return NULL;
	}

	void *elemento_buscado = NULL;
	nodo_t *nodo_actual = lista->nodo_inicio;
	size_t contador = 0;

	while (contador < posicion) {
		nodo_actual = nodo_actual->siguiente;
		contador++;
	}
	elemento_buscado = nodo_actual->elemento;

	return elemento_buscado;
}

void *lista_buscar_elemento(lista_t *lista, int (*comparador)(void *, void *),
			    void *contexto)
{
	if (lista == NULL || lista->longitud == 0 || comparador == NULL) {
		return NULL;
	}
	nodo_t *actual = lista->nodo_inicio;

	while (actual != NULL) {
		if (comparador(actual->elemento, contexto) == 0) {
			return actual->elemento;
		}
		actual = actual->siguiente;
	}

	return NULL;
}

void *lista_primero(lista_t *lista)
{
	if (lista == NULL || lista->longitud == 0) {
		return NULL;
	}
	return lista->nodo_inicio->elemento;
}

void *lista_ultimo(lista_t *lista)
{
	if (lista == NULL || lista->longitud == 0) {
		return NULL;
	}
	return lista->nodo_final->elemento;
}

bool lista_vacia(lista_t *lista)
{
	if (lista == NULL || lista->longitud == 0) {
		return true;
	}
	return false;
}

size_t lista_tamanio(lista_t *lista)
{
	if (lista == NULL) {
		return 0;
	}

	return lista->longitud;
}

void lista_destruir(lista_t *lista)
{
	if (lista == NULL) {
		return;
	}
	nodo_t *actual_nodo = lista->nodo_inicio;
	nodo_t *siguiente_nodo = NULL;

	while (actual_nodo != NULL) {
		siguiente_nodo = actual_nodo->siguiente;
		free(actual_nodo);
		actual_nodo = siguiente_nodo;
	}

	free(lista);
}

void lista_destruir_todo(lista_t *lista, void (*funcion)(void *))
{
	if (lista == NULL) {
		return;
	}

	nodo_t *actual_nodo = lista->nodo_inicio;
	nodo_t *siguiente_nodo = NULL;

	while (actual_nodo != NULL) {
		siguiente_nodo = actual_nodo->siguiente;

		if (funcion != NULL) {
			funcion(actual_nodo->elemento);
		}

		free(actual_nodo);
		actual_nodo = siguiente_nodo;
	}

	free(lista);
}

lista_iterador_t *lista_iterador_crear(lista_t *lista)
{
	if (lista == NULL) {
		return NULL;
	}

	lista_iterador_t *iterador = malloc(sizeof(lista_iterador_t));
	if (iterador == NULL) {
		return NULL;
	}
	iterador->actual = lista->nodo_inicio;

	return iterador;
}

bool lista_iterador_tiene_siguiente(lista_iterador_t *iterador)
{
	if (iterador == NULL || iterador->actual == NULL) {
		return false;
	}
	return true;
}

bool lista_iterador_avanzar(lista_iterador_t *iterador)
{
	if (iterador == NULL) {
		return false;
	}
	if (iterador->actual == NULL) {
		return false;
	}
	iterador->actual = iterador->actual->siguiente;

	if (iterador->actual == NULL) {
		return false;
	}
	return true;
}

void *lista_iterador_elemento_actual(lista_iterador_t *iterador)
{
	if (iterador == NULL || iterador->actual == NULL) {
		return NULL;
	}
	return iterador->actual->elemento;
}

void lista_iterador_destruir(lista_iterador_t *iterador)
{
	free(iterador);
	return;
}

size_t lista_con_cada_elemento(lista_t *lista, bool (*funcion)(void *, void *),
			       void *contexto)
{
	if (lista == NULL || funcion == NULL) {
		return 0;
	}

	size_t cant_elementos_iterados = 0;
	bool continuo_iterando = true;
	nodo_t *nodo = lista->nodo_inicio;

	while (nodo != NULL && cant_elementos_iterados < lista->longitud &&
	       continuo_iterando) {
		continuo_iterando = funcion(nodo->elemento, contexto);
		nodo = nodo->siguiente;
		cant_elementos_iterados++;
	}

	return cant_elementos_iterados;
}
