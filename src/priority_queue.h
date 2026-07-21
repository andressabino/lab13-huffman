#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "huffman_tree.h"

typedef struct {
    NodoHuffman** arreglo;
    unsigned int capacidad;
    unsigned int tamano;
} ColaPrioridad;

// Funciones para la cola de prioridad
ColaPrioridad* crear_cola(unsigned int capacidad);
void destruir_cola(ColaPrioridad* cola);
void insertar_cola(ColaPrioridad* cola, NodoHuffman* nodo);
NodoHuffman* extraer_minimo(ColaPrioridad* cola);
int cola_vacia(ColaPrioridad* cola);
void heapify(ColaPrioridad* cola, unsigned int indice);

#endif