#include "huffman_tree.h"
#include "priority_queue.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

NodoHuffman* crear_nodo(unsigned char caracter, unsigned int frecuencia, 
                        NodoHuffman* izq, NodoHuffman* der) {
    NodoHuffman* nodo = (NodoHuffman*)malloc(sizeof(NodoHuffman));
    if (!nodo) {
        printf("Error: No se pudo crear el nodo\n");
        return NULL;
    }
    nodo->caracter = caracter;
    nodo->frecuencia = frecuencia;
    nodo->izq = izq;
    nodo->der = der;
    
    if (izq && der) {
        nodo->min_caracter = (izq->min_caracter < der->min_caracter) ? izq->min_caracter : der->min_caracter;
    } else if (izq) {
        nodo->min_caracter = izq->min_caracter;
    } else if (der) {
        nodo->min_caracter = der->min_caracter;
    } else {
        nodo->min_caracter = (unsigned int)caracter;
    }
    
    return nodo;
}

void destruir_arbol(NodoHuffman* raiz) {
    if (raiz) {
        destruir_arbol(raiz->izq);
        destruir_arbol(raiz->der);
        free(raiz);
    }
}

NodoHuffman* construir_arbol(unsigned int* tabla_frecuencias) {
    ColaPrioridad* cola = crear_cola(256);
    if (!cola) return NULL;
    
    // Insertar todos los caracteres con frecuencia > 0
    for (int i = 0; i < 256; i++) {
        if (tabla_frecuencias[i] > 0) {
            NodoHuffman* nodo = crear_nodo((unsigned char)i, tabla_frecuencias[i], NULL, NULL);
            if (nodo) {
                insertar_cola(cola, nodo);
            }
        }
    }
    
    // Si no hay caracteres en la tabla (archivo vacío)
    if (cola->tamano == 0) {
        destruir_cola(cola);
        return NULL;
    }
    
    // Si solo hay un carácter, crear un árbol con un nodo interno
    if (cola->tamano == 1) {
        NodoHuffman* unico = extraer_minimo(cola);
        NodoHuffman* padre = crear_nodo(0, unico->frecuencia, unico, NULL);
        destruir_cola(cola);
        return padre;
    }
    
    // Construir el árbol fusionando nodos
    while (cola->tamano > 1) {
        NodoHuffman* izq = extraer_minimo(cola);
        NodoHuffman* der = extraer_minimo(cola);
        
        NodoHuffman* padre = crear_nodo(0, izq->frecuencia + der->frecuencia, izq, der);
        if (padre) {
            insertar_cola(cola, padre);
        }
    }
    
    NodoHuffman* raiz = extraer_minimo(cola);
    destruir_cola(cola);
    return raiz;
}

void generar_codigos(NodoHuffman* raiz, char** diccionario, char* codigo_actual, int profundidad) {
    if (!raiz) return;
    
    if (es_hoja(raiz)) {
        // Asignar código a la hoja
        diccionario[raiz->caracter] = (char*)malloc(profundidad + 1);
        if (diccionario[raiz->caracter]) {
            strcpy(diccionario[raiz->caracter], codigo_actual);
        }
        return;
    }
    
    // Recorrer hijo izquierdo (0)
    if (raiz->izq) {
        codigo_actual[profundidad] = '0';
        codigo_actual[profundidad + 1] = '\0';
        generar_codigos(raiz->izq, diccionario, codigo_actual, profundidad + 1);
    }
    
    // Recorrer hijo derecho (1)
    if (raiz->der) {
        codigo_actual[profundidad] = '1';
        codigo_actual[profundidad + 1] = '\0';
        generar_codigos(raiz->der, diccionario, codigo_actual, profundidad + 1);
    }
}

int es_hoja(NodoHuffman* nodo) {
    return nodo && !nodo->izq && !nodo->der;
}