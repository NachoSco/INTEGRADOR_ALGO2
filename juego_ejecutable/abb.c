#include "abb.h"
#include "abb_estructura_privada.h"
#include <stddef.h>
#include <stdlib.h>

typedef struct {
	void **array;
	size_t tamanio;
	size_t longitud_actual;

} estructura_vectorial_t;

estructura_vectorial_t *crear_estructura_vectorial(void **array, size_t tamanio)
{
	estructura_vectorial_t *nueva_estructura =
		calloc(1, sizeof(estructura_vectorial_t));
	if (nueva_estructura == NULL) {
		return NULL;
	}

	nueva_estructura->array = array;
	nueva_estructura->tamanio = tamanio;

	return nueva_estructura;
}

nodo_abb_t *nodo_abb_crear(void *elemento)
{
	nodo_abb_t *nuevo_nodo = calloc(1, sizeof(nodo_abb_t));
	if (nuevo_nodo == NULL) {
		return NULL;
	}

	nuevo_nodo->elemento = elemento;
	return nuevo_nodo;
}

abb_t *abb_crear(abb_comparador comparador)
{
	if (comparador == NULL) {
		return NULL;
	}

	abb_t *nuevo_abb = calloc(1, sizeof(abb_t));
	if (nuevo_abb == NULL) {
		return NULL;
	}

	nuevo_abb->comparador = comparador;
	return nuevo_abb;
}

void funcion_insertar_recursiva(nodo_abb_t *nodo_actual, void *elemento,
				abb_comparador comparador)
{
	int resultado_de_comparar = comparador(elemento, nodo_actual->elemento);
	//inserta en la izq
	if (resultado_de_comparar < 0) {
		if (nodo_actual->izquierda == NULL) {
			nodo_actual->izquierda = nodo_abb_crear(elemento);
		} else {
			funcion_insertar_recursiva(nodo_actual->izquierda,
						   elemento, comparador);
		}
		//inserta en la derecha
	} else if (resultado_de_comparar > 0) {
		if (nodo_actual->derecha == NULL) {
			nodo_actual->derecha = nodo_abb_crear(elemento);
		} else {
			funcion_insertar_recursiva(nodo_actual->derecha,
						   elemento, comparador);
		}
		//si el elemento es el mismo que la raiz, insertalo a la izq
	} else {
		if (nodo_actual->izquierda == NULL) {
			nodo_actual->izquierda = nodo_abb_crear(elemento);
		} else {
			funcion_insertar_recursiva(nodo_actual->izquierda,
						   elemento, comparador);
		}
	}
}

abb_t *abb_insertar(abb_t *arbol, void *elemento)
{
	if (arbol == NULL) {
		return NULL;
	}
	if (arbol->tamanio == 0) {
		arbol->nodo_raiz = nodo_abb_crear(elemento);
	} else {
		funcion_insertar_recursiva(arbol->nodo_raiz, elemento,
					   arbol->comparador);
	}
	arbol->tamanio++;
	return arbol;
}

void *funcion_aux_buscar(nodo_abb_t *nodo_actual, void *elemento,
			 abb_comparador comparador)
{
	if (nodo_actual == NULL) {
		return NULL;
	}

	int resultado_de_comparar = comparador(elemento, nodo_actual->elemento);

	if (resultado_de_comparar < 0) {
		return funcion_aux_buscar(nodo_actual->izquierda, elemento,
					  comparador);
	} else if (resultado_de_comparar > 0) {
		return funcion_aux_buscar(nodo_actual->derecha, elemento,
					  comparador);
	} else {
		return nodo_actual->elemento;
	}
}

void *abb_buscar(abb_t *arbol, void *elemento)
{
	if (arbol == NULL)
		return NULL;
	return funcion_aux_buscar(arbol->nodo_raiz, elemento,
				  arbol->comparador);
}

void buscar_nodo_y_padre(nodo_abb_t *nodo_actual, void *elemento,
			 abb_comparador comparador, nodo_abb_t **nodo_eliminar,
			 nodo_abb_t **padre)
{
	*padre = NULL;
	*nodo_eliminar = NULL;

	while (nodo_actual != NULL) {
		int comparacion = comparador(elemento, nodo_actual->elemento);

		if (comparacion == 0) {
			*nodo_eliminar = nodo_actual;
			return;
		} else if (comparacion < 0) {
			*padre = nodo_actual;
			nodo_actual = nodo_actual->izquierda;
		} else {
			*padre = nodo_actual;
			nodo_actual = nodo_actual->derecha;
		}
	}
}

void eliminar_hoja(nodo_abb_t *nodo_padre_eliminar, nodo_abb_t *nodo_a_eliminar)
{
	if (nodo_padre_eliminar->izquierda == nodo_a_eliminar) {
		nodo_padre_eliminar->izquierda = NULL;
	} else {
		nodo_padre_eliminar->derecha = NULL;
	}
	free(nodo_a_eliminar);
}

void eliminar_un_hijo(nodo_abb_t *nodo_padre_eliminar,
		      nodo_abb_t *nodo_a_eliminar)
{
	nodo_abb_t *nodo_hijo = NULL;
	if (nodo_a_eliminar->izquierda != NULL) {
		nodo_hijo = nodo_a_eliminar->izquierda;
	} else {
		nodo_hijo = nodo_a_eliminar->derecha;
	}

	if (nodo_padre_eliminar->izquierda == nodo_a_eliminar) {
		nodo_padre_eliminar->izquierda = nodo_hijo;
	} else {
		nodo_padre_eliminar->derecha = nodo_hijo;
	}

	free(nodo_a_eliminar);
}

void encontrar_predecesor_inorden_y_padre(nodo_abb_t *nodo_a_eliminar,
					  nodo_abb_t **predecesor_inorden,
					  nodo_abb_t **padre_predecesor)
{
	*predecesor_inorden = nodo_a_eliminar->izquierda;
	*padre_predecesor = nodo_a_eliminar;

	// Buscar el nodo más a la derecha en el subárbol izquierdo (predecesor inorden) y su padre
	while ((*predecesor_inorden)->derecha != NULL) {
		*padre_predecesor = *predecesor_inorden;
		*predecesor_inorden = (*predecesor_inorden)->derecha;
	}
}

void eliminar_dos_hijos(abb_t *arbol, nodo_abb_t *nodo_padre_eliminar,
			nodo_abb_t *nodo_a_eliminar)
{
	// Crear punteros para el predecesor inorden y su padre
	nodo_abb_t *predecesor_inorden;
	nodo_abb_t *padre_predecesor;

	// Encontrar el predecesor inorden y su padre
	encontrar_predecesor_inorden_y_padre(
		nodo_a_eliminar, &predecesor_inorden, &padre_predecesor);

	// El padre del predecesor inorden tiene que apuntar a NULL
	if (padre_predecesor->izquierda == predecesor_inorden) {
		padre_predecesor->izquierda = NULL;
	} else {
		padre_predecesor->derecha = NULL;
	}

	// El padre del nodo a eliminar tiene que apuntar al predecesor inorden
	if (nodo_padre_eliminar->izquierda == nodo_a_eliminar) {
		nodo_padre_eliminar->izquierda = predecesor_inorden;
	} else {
		nodo_padre_eliminar->derecha = predecesor_inorden;
	}

	// El predecesor inorden tiene que apuntar a los hijos del nodo a eliminar
	predecesor_inorden->izquierda = nodo_a_eliminar->izquierda;
	predecesor_inorden->derecha = nodo_a_eliminar->derecha;

	// Liberar memoria del nodo a eliminar
	free(nodo_a_eliminar);
}

void eliminar_nodo(abb_t *arbol, nodo_abb_t *nodo_padre_eliminar,
		   nodo_abb_t *nodo_a_eliminar)
{
	// Caso 1: Nodo a eliminar es una hoja
	if (nodo_a_eliminar->izquierda == NULL &&
	    nodo_a_eliminar->derecha == NULL) {
		eliminar_hoja(nodo_padre_eliminar, nodo_a_eliminar);
	}
	// Caso 2: Nodo a eliminar tiene un solo hijo
	else if (nodo_a_eliminar->izquierda == NULL ||
		 nodo_a_eliminar->derecha == NULL) {
		eliminar_un_hijo(nodo_padre_eliminar, nodo_a_eliminar);
	}
	// Caso 3: Nodo a eliminar tiene dos hijos
	else {
		eliminar_dos_hijos(arbol, nodo_padre_eliminar, nodo_a_eliminar);
	}
}

void eliminar_nodo_raiz(abb_t *arbol)
{
	nodo_abb_t *nodo_raiz = arbol->nodo_raiz;

	// Si el nodo raíz no tiene hijos o solo tiene un hijo derecho, simplemente lo liberamos y establecemos el nodo raíz como NULL
	if (nodo_raiz->izquierda == NULL) {
		arbol->nodo_raiz = nodo_raiz->derecha;
		free(nodo_raiz);
		return;
	}

	// Si el nodo raíz tiene un solo hijo izquierdo o dos hijos, encontramos el predecesor inorden y su padre
	nodo_abb_t *predecesor_inorden;
	nodo_abb_t *padre_predecesor;

	encontrar_predecesor_inorden_y_padre(nodo_raiz, &predecesor_inorden,
					     &padre_predecesor);

	// El predecesor inorden se convierte en la nueva raíz del árbol
	if (padre_predecesor != nodo_raiz) {
		padre_predecesor->derecha = predecesor_inorden->izquierda;
		predecesor_inorden->izquierda = nodo_raiz->izquierda;
	}

	predecesor_inorden->derecha = nodo_raiz->derecha;
	arbol->nodo_raiz = predecesor_inorden;

	// Liberar memoria del nodo raíz
	free(nodo_raiz);
}

bool funcion_aux_buscar_2(nodo_abb_t *nodo_actual, void *elemento,
			  abb_comparador comparador)
{
	if (nodo_actual == NULL) {
		return false;
	}

	int comparacion = comparador(elemento, nodo_actual->elemento);

	if (comparacion < 0) {
		return funcion_aux_buscar(nodo_actual->izquierda, elemento,
					  comparador);
	} else if (comparacion > 0) {
		return funcion_aux_buscar(nodo_actual->derecha, elemento,
					  comparador);
	} else {
		return true;
	}
}

void *abb_quitar(abb_t *arbol, void *elemento)
{
	if (arbol == NULL) {
		return NULL;
	}
	//veo si el elemento esta en el arbol
	if (funcion_aux_buscar_2(arbol->nodo_raiz, elemento,
				 arbol->comparador)) {
		//el elemnto esta en el arbol asi que busco el padre
		nodo_abb_t *padre = NULL;
		nodo_abb_t *nodo_eliminar = NULL;

		buscar_nodo_y_padre(arbol->nodo_raiz, elemento,
				    arbol->comparador, &nodo_eliminar, &padre);

		if (padre == NULL) {
			// Caso especial: eliminar raíz
			eliminar_nodo_raiz(arbol);
		} else {
			// Llamar a la función para eliminar el nodo
			eliminar_nodo(arbol, padre, nodo_eliminar);
		}
		arbol->tamanio--;
		return elemento;
	}
	return NULL;
}

bool abb_vacio(abb_t *arbol)
{
	if (arbol == NULL || arbol->tamanio == 0)
		return true;

	return false;
}

size_t abb_tamanio(abb_t *arbol)
{
	if (arbol == NULL)
		return 0;
	return arbol->tamanio;
}

void funcion_aux_destruir(nodo_abb_t *nodo)
{
	if (nodo == NULL) {
		return;
	}
	funcion_aux_destruir(nodo->derecha);
	funcion_aux_destruir(nodo->izquierda);
	free(nodo);
}

void abb_destruir(abb_t *arbol)
{
	if (arbol == NULL) {
		return;
	}
	funcion_aux_destruir(arbol->nodo_raiz);
	free(arbol);
}

void funcion_aux_destruir_todo(nodo_abb_t *nodo, void (*destructor)(void *))
{
	if (nodo == NULL) {
		return;
	}

	funcion_aux_destruir_todo(nodo->derecha, destructor);
	funcion_aux_destruir_todo(nodo->izquierda, destructor);

	if (destructor != NULL) {
		destructor(nodo->elemento);
	}
	free(nodo);
}

void abb_destruir_todo(abb_t *arbol, void (*destructor)(void *))
{
	if (arbol == NULL) {
		return;
	}

	funcion_aux_destruir_todo(arbol->nodo_raiz, destructor);
	free(arbol);
}

// IZQUIERDO-ACTUAL-DERECHO
size_t abb_inorden(nodo_abb_t *nodo_actual, bool (*funcion)(void *, void *),
		   void *aux, bool *continuar)
{
	if (nodo_actual == NULL || !continuar) {
		return 0;
	}

	size_t contador = 0;
	contador +=
		abb_inorden(nodo_actual->izquierda, funcion, aux, continuar);

	if (*continuar) {
		*continuar = funcion(nodo_actual->elemento, aux);
		contador++;
	}

	if (!*continuar) {
		return contador;
	}

	contador += abb_inorden(nodo_actual->derecha, funcion, aux, continuar);

	return contador;
}

size_t abb_preorden(nodo_abb_t *nodo_actual, bool (*funcion)(void *, void *),
		    void *aux, bool *continuar)
{
	if (nodo_actual == NULL || !*continuar) {
		return 0;
	}

	size_t contador = 0;

	bool resultado = funcion(nodo_actual->elemento, aux);
	contador++;

	if (!resultado) {
		*continuar = false;
		return contador;
	}

	contador +=
		abb_preorden(nodo_actual->izquierda, funcion, aux, continuar);
	contador += abb_preorden(nodo_actual->derecha, funcion, aux, continuar);

	return contador;
}

size_t abb_postorden(nodo_abb_t *nodo_actual, bool (*funcion)(void *, void *),
		     void *aux, bool *continuar)
{
	if (nodo_actual == NULL) {
		return 0;
	}

	size_t contador = 0;

	contador +=
		abb_postorden(nodo_actual->izquierda, funcion, aux, continuar);
	contador +=
		abb_postorden(nodo_actual->derecha, funcion, aux, continuar);

	if (*continuar) {
		bool resultado = funcion(nodo_actual->elemento, aux);
		contador++;

		if (!resultado) {
			*continuar = false;
		}
	}

	return contador;
}

size_t abb_con_cada_elemento(abb_t *arbol, abb_recorrido recorrido,
			     bool (*funcion)(void *, void *), void *aux)
{
	if (arbol == NULL || funcion == NULL) {
		return 0;
	}

	size_t contador = 0;
	bool continuar = true;

	switch (recorrido) {
	case INORDEN:
		contador =
			abb_inorden(arbol->nodo_raiz, funcion, aux, &continuar);
		break;
	case PREORDEN:
		contador = abb_preorden(arbol->nodo_raiz, funcion, aux,
					&continuar);
		break;
	case POSTORDEN:
		contador = abb_postorden(arbol->nodo_raiz, funcion, aux,
					 &continuar);
		break;
	default:
		break;
	}

	return contador;
}

void inorden_recorrer(nodo_abb_t *nodo_actual, void **array,
		      size_t tamanio_array, size_t *contador)
{
	if (!nodo_actual || *contador >= tamanio_array)
		return;

	inorden_recorrer(nodo_actual->izquierda, array, tamanio_array,
			 contador);

	if (*contador < tamanio_array) {
		array[*contador] = nodo_actual->elemento;
		*contador = *contador + 1;
	}

	inorden_recorrer(nodo_actual->derecha, array, tamanio_array, contador);
}

void preorden_recorrer(nodo_abb_t *nodo_actual, void **array,
		       size_t tamanio_array, size_t *contador)
{
	if (!nodo_actual || *contador >= tamanio_array)
		return;

	if (*contador < tamanio_array) {
		array[*contador] = nodo_actual->elemento;
		*contador = *contador + 1;
	}

	preorden_recorrer(nodo_actual->izquierda, array, tamanio_array,
			  contador);

	preorden_recorrer(nodo_actual->derecha, array, tamanio_array, contador);
}

void postorden_recorrer(nodo_abb_t *nodo_actual, void **array,
			size_t tamanio_array, size_t *contador)
{
	if (!nodo_actual || *contador >= tamanio_array)
		return;

	postorden_recorrer(nodo_actual->izquierda, array, tamanio_array,
			   contador);

	postorden_recorrer(nodo_actual->derecha, array, tamanio_array,
			   contador);

	if (*contador < tamanio_array) {
		array[*contador] = nodo_actual->elemento;
		*contador = *contador + 1;
	}
}

void abb_recorrer_rec(nodo_abb_t *nodo_actual, abb_recorrido recorrido,
		      void **array, size_t tamanio_array, size_t *contador)
{
	if (!nodo_actual || *contador >= tamanio_array)
		return;

	switch (recorrido) {
	case PREORDEN:
		preorden_recorrer(nodo_actual, array, tamanio_array, contador);
		break;

	case INORDEN:
		inorden_recorrer(nodo_actual, array, tamanio_array, contador);
		break;

	case POSTORDEN:
		postorden_recorrer(nodo_actual, array, tamanio_array, contador);
		break;

	default:
		break;
	}
	return;
}

size_t abb_recorrer(abb_t *arbol, abb_recorrido recorrido, void **array,
		    size_t tamanio_array)
{
	size_t contador = 0;
	if (arbol && array)
		abb_recorrer_rec(arbol->nodo_raiz, recorrido, array,
				 tamanio_array, &contador);
	return contador;
}