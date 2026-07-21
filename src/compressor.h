#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <stdio.h>

// Funciones de compresión
void contar_frecuencias(FILE* archivo, unsigned int* tabla_frecuencias);
void comprimir_archivo(const char* archivo_entrada, const char* archivo_salida);
void descomprimir_archivo(const char* archivo_comprimido, const char* archivo_destino);
void liberar_diccionario(char** diccionario);
#endif