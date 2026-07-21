#include "priority_queue.h"
#include <stdlib.h>
#include <stdio.h>

static int comparar_nodos(NodoHuffman* a, NodoHuffman* b) {
    if (a->frecuencia != b->frecuencia) {
        return (a->frecuencia < b->frecuencia) ? -1 : 1;
    }
    // Desempate 100% determinista por el carácter mínimo del subárbol:
    if (a->min_caracter != b->min_caracter) {
        return (a->min_caracter < b->min_caracter) ? -1 : 1;
    }
    return 0;
}

ColaPrioridad* crear_cola(unsigned int capacidad) {
    ColaPrioridad* cola = (ColaPrioridad*)malloc(sizeof(ColaPrioridad));
    if (!cola) {
        printf("Error: No se pudo crear la cola\n");
        return NULL;
    }
    
    cola->arreglo = (NodoHuffman**)malloc(sizeof(NodoHuffman*) * capacidad);
    if (!cola->arreglo) {
        free(cola);
        printf("Error: No se pudo asignar memoria para el arreglo\n");
        return NULL;
    }
    
    cola->capacidad = capacidad;
    cola->tamano = 0;
    return cola;
}

void destruir_cola(ColaPrioridad* cola) {
    if (cola) {
        if (cola->arreglo) {
            free(cola->arreglo);
        }
        free(cola);
    }
}

void heapify(ColaPrioridad* cola, unsigned int indice) {
    unsigned int menor = indice;
    unsigned int izquierda = 2 * indice + 1;
    unsigned int derecha = 2 * indice + 2;
    
    if (izquierda < cola->tamano && 
        comparar_nodos(cola->arreglo[izquierda], cola->arreglo[menor]) < 0) {
        menor = izquierda;
    }
    
    if (derecha < cola->tamano && 
        comparar_nodos(cola->arreglo[derecha], cola->arreglo[menor]) < 0) {
        menor = derecha;
    }
    
    if (menor != indice) {
        NodoHuffman* temp = cola->arreglo[indice];
        cola->arreglo[indice] = cola->arreglo[menor];
        cola->arreglo[menor] = temp;
        heapify(cola, menor);
    }
}

void insertar_cola(ColaPrioridad* cola, NodoHuffman* nodo) {
    if (cola->tamano >= cola->capacidad) {
        printf("Error: Cola llena\n");
        return;
    }
    
    unsigned int i = cola->tamano;
    cola->tamano++;
    cola->arreglo[i] = nodo;
    
    while (i > 0 && comparar_nodos(cola->arreglo[i], cola->arreglo[(i - 1) / 2]) < 0) {
        NodoHuffman* temp = cola->arreglo[i];
        cola->arreglo[i] = cola->arreglo[(i - 1) / 2];
        cola->arreglo[(i - 1) / 2] = temp;
        i = (i - 1) / 2;
    }
}

NodoHuffman* extraer_minimo(ColaPrioridad* cola) {
    if (cola->tamano == 0) {
        return NULL;
    }
    
    NodoHuffman* raiz = cola->arreglo[0];
    cola->arreglo[0] = cola->arreglo[cola->tamano - 1];
    cola->tamano--;
    heapify(cola, 0);
    return raiz;
}

int cola_vacia(ColaPrioridad* cola) {
    return cola->tamano == 0;
}