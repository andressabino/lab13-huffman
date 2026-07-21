#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "compressor.h"

void mostrar_ayuda() {
    printf("Uso:\n");
    printf("  Compresion:   ./huffman -c archivo.txt archivo.huff\n");
    printf("  Descompresion: ./huffman -d archivo.huff archivo_recuperado.txt\n");
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Error: Numero incorrecto de argumentos\n");
        mostrar_ayuda();
        return 1;
    }
    
    if (strcmp(argv[1], "-c") == 0) {
        // Modo compresion
        printf("Comprimiendo %s a %s...\n", argv[2], argv[3]);
        comprimir_archivo(argv[2], argv[3]);
    } else if (strcmp(argv[1], "-d") == 0) {
        // Modo descompresion
        printf("Descomprimiendo %s a %s...\n", argv[2], argv[3]);
        descomprimir_archivo(argv[2], argv[3]);
    } else {
        printf("Error: Opcion invalida. Use -c o -d\n");
        mostrar_ayuda();
        return 1;
    }
    
    return 0;
}