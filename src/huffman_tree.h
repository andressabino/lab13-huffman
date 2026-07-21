#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

typedef struct NodoHuffman {
    unsigned char caracter;
    unsigned int frecuencia;
    unsigned int min_caracter;
    struct NodoHuffman* izq;
    struct NodoHuffman* der;
} NodoHuffman;

// Funciones para el árbol de Huffman
NodoHuffman* crear_nodo(unsigned char caracter, unsigned int frecuencia, 
                        NodoHuffman* izq, NodoHuffman* der);
void destruir_arbol(NodoHuffman* raiz);
NodoHuffman* construir_arbol(unsigned int* tabla_frecuencias);
void generar_codigos(NodoHuffman* raiz, char** diccionario, char* codigo_actual, int profundidad);
int es_hoja(NodoHuffman* nodo);

#endif