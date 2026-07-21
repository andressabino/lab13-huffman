#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compressor.h"

// Funcion auxiliar para comparar dos archivos byte por byte
static int archivos_son_identicos(const char* ruta1, const char* ruta2) {
    FILE* f1 = fopen(ruta1, "rb");
    FILE* f2 = fopen(ruta2, "rb");
    
    if (!f1 || !f2) {
        if (f1) fclose(f1);
        if (f2) fclose(f2);
        return 0;
    }
    
    int igual = 1;
    int b1, b2;
    do {
        b1 = fgetc(f1);
        b2 = fgetc(f2);
        if (b1 != b2) {
            igual = 0;
            break;
        }
    } while (b1 != EOF && b2 != EOF);
    
    fclose(f1);
    fclose(f2);
    return igual;
}

// Generadores de archivos de prueba
static void crear_archivo_vacio(const char* ruta) {
    FILE* f = fopen(ruta, "wb");
    if (f) fclose(f);
}

static void crear_archivo_un_caracter(const char* ruta) {
    FILE* f = fopen(ruta, "wb");
    if (f) {
        for (int i = 0; i < 500; i++) {
            fputc('A', f);
        }
        fclose(f);
    }
}

static void crear_archivo_256_caracteres(const char* ruta) {
    FILE* f = fopen(ruta, "wb");
    if (f) {
        for (int i = 0; i < 256; i++) {
            fputc((unsigned char)i, f);
        }
        fclose(f);
    }
}

static void crear_archivo_frecuencias_iguales(const char* ruta) {
    FILE* f = fopen(ruta, "wb");
    if (f) {
        // 100 repeticiones de cada letra entre 'A' y 'J' (10 caracteres distintos)
        for (int rep = 0; rep < 100; rep++) {
            for (char c = 'A'; c <= 'J'; c++) {
                fputc(c, f);
            }
        }
        fclose(f);
    }
}

static void probar_caso(const char* nombre_caso, const char* ruta_orig, const char* ruta_huff, const char* ruta_recup) {
    printf("=========================================\n");
    printf("EJECUTANDO CASO DE PRUEBA: %s\n", nombre_caso);
    printf("=========================================\n");
    
    comprimir_archivo(ruta_orig, ruta_huff);
    descomprimir_archivo(ruta_huff, ruta_recup);
    
    if (archivos_son_identicos(ruta_orig, ruta_recup)) {
        printf("[OK PASO] El archivo recuperado es 100%% identico al original.\n\n");
    } else {
        printf("[ERROR FALLO] El archivo recuperado DIFIERE del original.\n\n");
    }
}

int main() {
    printf("=========================================\n");
    printf("   SUITE DE PRUEBAS LAB 13 HUFFMAN      \n");
    printf("=========================================\n\n");
    
    // Rutas de prueba
    const char* orig_vac = "resultados/test_vacio.txt";
    const char* huff_vac = "resultados/test_vacio.huff";
    const char* recup_vac = "resultados/test_vacio_recup.txt";
    
    const char* orig_un = "resultados/test_un_caracter.txt";
    const char* huff_un = "resultados/test_un_caracter.huff";
    const char* recup_un = "resultados/test_un_caracter_recup.txt";
    
    const char* orig_256 = "resultados/test_256.bin";
    const char* huff_256 = "resultados/test_256.huff";
    const char* recup_256 = "resultados/test_256_recup.bin";
    
    const char* orig_eq = "resultados/test_eq.txt";
    const char* huff_eq = "resultados/test_eq.huff";
    const char* recup_eq = "resultados/test_eq_recup.txt";
    
    const char* huff_txt = "resultados/texto.huff";
    const char* recup_txt = "resultados/texto_recuperado.txt";
    
    // Crear archivos de prueba
    crear_archivo_vacio(orig_vac);
    crear_archivo_un_caracter(orig_un);
    crear_archivo_256_caracteres(orig_256);
    crear_archivo_frecuencias_iguales(orig_eq);
    
    // Probar archivo original si existe (texto.txt o archivo.txt)
    const char* orig_txt = "texto.txt";
    FILE* f_check = fopen("texto.txt", "rb");
    if (!f_check) {
        f_check = fopen("archivo.txt", "rb");
        if (f_check) orig_txt = "archivo.txt";
    }
    
    if (f_check) {
        fclose(f_check);
        probar_caso("1. Archivo Original de Texto", orig_txt, huff_txt, recup_txt);
    }
    
    probar_caso("2. Archivo Vacio (0 bytes)", orig_vac, huff_vac, recup_vac);
    probar_caso("3. Archivo Un Solo Caracter Repetido", orig_un, huff_un, recup_un);
    probar_caso("4. Archivo Con Todos Los 256 Caracteres", orig_256, huff_256, recup_256);
    probar_caso("5. Archivo Frecuencias Iguales (Determinismo Heap)", orig_eq, huff_eq, recup_eq);
    
    printf("=========================================\n");
    printf("   TODAS LAS PRUEBAS FINALIZADAS        \n");
    printf("=========================================\n");
    
    return 0;
}
