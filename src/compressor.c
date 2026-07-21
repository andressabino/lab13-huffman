#include "compressor.h"
#include "huffman_tree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

void imprimir_metricas(unsigned int* tabla_frecuencias, char** diccionario, long tamano_original, long tamano_comprimido) {
    unsigned long total_caracteres = 0;
    for (int i = 0; i < 256; i++) {
        total_caracteres += tabla_frecuencias[i];
    }
    
    double entropia = 0.0;
    double longitud_media = 0.0;
    
    if (total_caracteres > 0) {
        for (int i = 0; i < 256; i++) {
            if (tabla_frecuencias[i] > 0) {
                double p_i = (double)tabla_frecuencias[i] / total_caracteres;
                entropia -= p_i * log2(p_i);
                
                if (diccionario && diccionario[i]) {
                    size_t len = strlen(diccionario[i]);
                    longitud_media += p_i * (double)len;
                }
            }
        }
    }
    
    double cr = (tamano_comprimido > 0) ? ((double)tamano_original / tamano_comprimido) : 0.0;
    double sp = (tamano_original > 0) ? ((1.0 - (double)tamano_comprimido / tamano_original) * 100.0) : 0.0;
    
    printf("\n=== METRICAS DE COMPRESION HUFFMAN ===\n");
    printf("  Tamano original:           %ld bytes\n", tamano_original);
    printf("  Tamano comprimido:          %ld bytes\n", tamano_comprimido);
    printf("  Entropia de la fuente (H):  %.4f bits/caracter\n", entropia);
    printf("  Longitud media codigo (L):  %.4f bits/caracter\n", longitud_media);
    printf("  Tasa de compresion (CR):    %.2f:1\n", cr);
    printf("  Porcentaje de ahorro (SP):  %.2f%%\n", sp);
    printf("=======================================\n\n");
}

void escribir_cabecera(FILE* salida, unsigned int* tabla_frecuencias) {
    int num_entradas = 0;
    for (int i = 0; i < 256; i++) {
        if (tabla_frecuencias[i] > 0) {
            num_entradas++;
        }
    }
    
    // Escribir N (0 representa 256)
    unsigned char N = (num_entradas == 256) ? 0 : (unsigned char)num_entradas;
    fwrite(&N, sizeof(unsigned char), 1, salida);
    
    // Escribir entradas de la tabla de frecuencias
    for (int i = 0; i < 256; i++) {
        if (tabla_frecuencias[i] > 0) {
            unsigned char caracter = (unsigned char)i;
            uint32_t frecuencia = (uint32_t)tabla_frecuencias[i];
            fwrite(&caracter, sizeof(unsigned char), 1, salida);
            fwrite(&frecuencia, sizeof(uint32_t), 1, salida);
        }
    }
}

void contar_frecuencias(FILE* archivo, unsigned int* tabla_frecuencias) {
    for (int i = 0; i < 256; i++) {
        tabla_frecuencias[i] = 0;
    }
    
    unsigned char byte;
    while (fread(&byte, sizeof(unsigned char), 1, archivo) == 1) {
        tabla_frecuencias[byte]++;
    }
    
    rewind(archivo);
}

void comprimir_archivo(const char* archivo_entrada, const char* archivo_salida) {
    FILE* entrada = fopen(archivo_entrada, "rb");
    if (!entrada) {
        printf("Error: No se pudo abrir el archivo de entrada: %s\n", archivo_entrada);
        return;
    }
    
    // Medir tamano del archivo de entrada
    fseek(entrada, 0, SEEK_END);
    long tamano_original = ftell(entrada);
    fseek(entrada, 0, SEEK_SET);
    
    // Contar frecuencias
    unsigned int tabla_frecuencias[256];
    contar_frecuencias(entrada, tabla_frecuencias);
    
    // Si el archivo esta vacio
    if (tamano_original == 0) {
        FILE* salida = fopen(archivo_salida, "wb");
        if (!salida) {
            printf("Error: No se pudo crear el archivo de salida: %s\n", archivo_salida);
            fclose(entrada);
            return;
        }
        unsigned char N = 0;
        unsigned char padding = 0;
        fwrite(&N, sizeof(unsigned char), 1, salida);
        fwrite(&padding, sizeof(unsigned char), 1, salida);
        fclose(entrada);
        fclose(salida);
        printf("Compresion de archivo vacio completada exitosamente\n");
        imprimir_metricas(tabla_frecuencias, NULL, 0, 2);
        return;
    }
    
    // Construir arbol de Huffman
    NodoHuffman* raiz = construir_arbol(tabla_frecuencias);
    if (!raiz) {
        fclose(entrada);
        printf("Error: No se pudo construir el arbol de Huffman\n");
        return;
    }
    
    // Generar diccionario de codigos
    char** diccionario = (char**)malloc(sizeof(char*) * 256);
    for (int i = 0; i < 256; i++) {
        diccionario[i] = NULL;
    }
    
    char codigo_actual[256];
    codigo_actual[0] = '\0';
    generar_codigos(raiz, diccionario, codigo_actual, 0);
    
    // Abrir archivo de salida
    FILE* salida = fopen(archivo_salida, "wb");
    if (!salida) {
        printf("Error: No se pudo crear el archivo de salida: %s\n", archivo_salida);
        fclose(entrada);
        destruir_arbol(raiz);
        liberar_diccionario(diccionario);
        return;
    }
    
    // Escribir cabecera
    escribir_cabecera(salida, tabla_frecuencias);
    
    // Guardar posicion para escribir el padding despues
    long pos_padding = ftell(salida);
    unsigned char padding = 0;
    fwrite(&padding, sizeof(unsigned char), 1, salida);
    
    // Comprimir datos
    unsigned char buffer = 0;
    int contador_bits = 0;
    unsigned char byte;
    
    rewind(entrada);
    while (fread(&byte, sizeof(unsigned char), 1, entrada) == 1) {
        char* codigo = diccionario[byte];
        if (codigo) {
            for (int i = 0; codigo[i] != '\0'; i++) {
                if (codigo[i] == '1') {
                    buffer = buffer | (1 << (7 - contador_bits));
                }
                contador_bits++;
                
                if (contador_bits == 8) {
                    fwrite(&buffer, sizeof(unsigned char), 1, salida);
                    buffer = 0;
                    contador_bits = 0;
                }
            }
        }
    }
    
    // Manejar ultimo byte
    if (contador_bits > 0) {
        fwrite(&buffer, sizeof(unsigned char), 1, salida);
        padding = 8 - contador_bits;
    }
    
    // Actualizar padding en la cabecera
    fseek(salida, pos_padding, SEEK_SET);
    fwrite(&padding, sizeof(unsigned char), 1, salida);
    
    fseek(salida, 0, SEEK_END);
    long tamano_comprimido = ftell(salida);
    
    // Cerrar archivos y liberar memoria
    fclose(entrada);
    fclose(salida);
    
    printf("Compresion completada exitosamente\n");
    imprimir_metricas(tabla_frecuencias, diccionario, tamano_original, tamano_comprimido);
    
    destruir_arbol(raiz);
    liberar_diccionario(diccionario);
}

void descomprimir_archivo(const char* archivo_comprimido, const char* archivo_destino) {
    FILE* comprimido = fopen(archivo_comprimido, "rb");
    if (!comprimido) {
        printf("Error: No se pudo abrir el archivo comprimido: %s\n", archivo_comprimido);
        return;
    }
    
    // Medir tamano total del archivo comprimido
    fseek(comprimido, 0, SEEK_END);
    long tamano_total = ftell(comprimido);
    fseek(comprimido, 0, SEEK_SET);
    
    if (tamano_total == 0) {
        FILE* destino = fopen(archivo_destino, "wb");
        if (destino) fclose(destino);
        fclose(comprimido);
        printf("Descompresion completada (archivo vacio)\n");
        return;
    }
    
    // Leer N
    unsigned char N;
    if (fread(&N, sizeof(unsigned char), 1, comprimido) != 1) {
        printf("Error: Archivo comprimido corrupto\n");
        fclose(comprimido);
        return;
    }
    
    // Determinar cantidad de entradas en la tabla
    int num_entradas = 0;
    if (N == 0) {
        if (tamano_total == 2) {
            num_entradas = 0; // Archivo vacio comprimido (1 byte N=0 + 1 byte padding=0)
        } else {
            num_entradas = 256; // 0 representa 256 entradas
        }
    } else {
        num_entradas = N;
    }
    
    // Reconstruir tabla de frecuencias
    unsigned int tabla_frecuencias[256];
    for (int i = 0; i < 256; i++) {
        tabla_frecuencias[i] = 0;
    }
    
    for (int i = 0; i < num_entradas; i++) {
        unsigned char caracter;
        uint32_t frecuencia;
        if (fread(&caracter, sizeof(unsigned char), 1, comprimido) != 1 ||
            fread(&frecuencia, sizeof(uint32_t), 1, comprimido) != 1) {
            printf("Error: Archivo comprimido corrupto al leer tabla\n");
            fclose(comprimido);
            return;
        }
        tabla_frecuencias[caracter] = frecuencia;
    }
    
    // Leer padding
    unsigned char padding = 0;
    if (fread(&padding, sizeof(unsigned char), 1, comprimido) != 1) {
        printf("Error: Archivo comprimido corrupto al leer padding\n");
        fclose(comprimido);
        return;
    }
    
    FILE* destino = fopen(archivo_destino, "wb");
    if (!destino) {
        printf("Error: No se pudo crear el archivo destino: %s\n", archivo_destino);
        fclose(comprimido);
        return;
    }
    
    // Caso de archivo vacio
    if (num_entradas == 0) {
        fclose(comprimido);
        fclose(destino);
        printf("Descompresion completada exitosamente (0 bytes)\n");
        return;
    }
    
    // Reconstruir arbol
    NodoHuffman* raiz = construir_arbol(tabla_frecuencias);
    if (!raiz) {
        fclose(comprimido);
        fclose(destino);
        printf("Error: No se pudo reconstruir el arbol de Huffman\n");
        return;
    }
    
    // Descomprimir datos
    NodoHuffman* nodo_actual = raiz;
    unsigned char byte;
    
    long bytes_datos_restantes = tamano_total - ftell(comprimido);
    
    while (bytes_datos_restantes > 0) {
        if (fread(&byte, sizeof(unsigned char), 1, comprimido) != 1) break;
        bytes_datos_restantes--;
        
        int bits_a_leer = 8;
        if (bytes_datos_restantes == 0) {
            bits_a_leer = 8 - padding;
        }
        
        for (int i = 0; i < bits_a_leer; i++) {
            int bit = (byte >> (7 - i)) & 1;
            
            if (bit == 0) {
                if (nodo_actual->izq) {
                    nodo_actual = nodo_actual->izq;
                }
            } else {
                if (nodo_actual->der) {
                    nodo_actual = nodo_actual->der;
                }
            }
            
            if (es_hoja(nodo_actual)) {
                fwrite(&nodo_actual->caracter, sizeof(unsigned char), 1, destino);
                nodo_actual = raiz;
            }
        }
    }
    
    // Cerrar archivos y liberar memoria
    fclose(comprimido);
    fclose(destino);
    destruir_arbol(raiz);
    
    printf("Descompresion completada exitosamente\n");
}

void liberar_diccionario(char** diccionario) {
    if (diccionario) {
        for (int i = 0; i < 256; i++) {
            if (diccionario[i]) {
                free(diccionario[i]);
            }
        }
        free(diccionario);
    }
}