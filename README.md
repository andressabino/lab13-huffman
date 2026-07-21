# Laboratorio 13: Compresion y Descompresion con Algoritmo de Huffman

Este repositorio contiene la implementacion completa, modular y determinista del **Algoritmo de Huffman** para la compresion y descompresion sin perdida de archivos de texto y binarios en lenguaje **C11**, correspondiente al lab 13



Estructura del Proyecto
El codigo se encuentra estructurado de forma modular de la siguiente manera:
```text
├── resultados/           # Archivos generados durante las pruebas (.huff, .txt, .bin)
├── src/                  # Archivos de codigo fuente (.c y .h)
│   ├── main.c            # Interfaz de linea de comandos (CLI)
│   ├── compressor.c/.h   # Algoritmos de compresion/descompresion y manejo de bitstream
│   ├── huffman_tree.c/.h # Creacion del arbol y generacion del diccionario (DFS)
│   ├── priority_queue.c/.h # Cola de prioridad basada en Min-Heap
│   └── test_runner.c     # Suite de pruebas automatizadas
├── archivo.txt           # Archivo de texto original de prueba
├── makefile              # Automatizacion de compilacion y ejecucion
└── README.md             # Documentacion del proyecto (este archivo)
```

---

 Instrucciones del Makefile
El `makefile` esta configurado para dar soporte nativo y multiplataforma tanto en **Windows** (PowerShell / CMD) como en **Linux/macOS**.

### 1. Compilar todo el proyecto
Genera los ejecutables `huffman` y `test_runner`:
```bash
make
```

### 2. Ejecutar suite de pruebas de casos borde
Compila y corre el ejecutable de pruebas, evaluando automaticamente los **5 casos borde** recomendados por la guia del laboratorio:
1. Archivo original de texto (`archivo.txt`).
2. Archivo vacio (`0 bytes`).
3. Archivo con un solo caracter repetido (500 'A's).
4. Archivo con los 256 bytes posibles (0x00 a 0xFF).
5. Archivo con frecuencias iguales (para verificar determinismo del Heap).
```bash
make test
```

### 3. Ejecutar compresion y descompresion rapida
Comprime `archivo.txt` a `resultados/archivo.huff` y lo descomprime a `resultados/archivo_recuperado.txt`:
```bash
make run
```

### 4. Limpiar archivos compilados y temporales
Borra los ejecutables, archivos objeto (`.o`) y los resultados de pruebas para dejar el repositorio limpio:
```bash
make clean
```

---

Sintaxis de la Interfaz por Linea de Comandos (CLI)
Una vez compilado, puedes interactuar directamente con el ejecutable `huffman` utilizando la siguiente sintaxis:

*   **Compresion**:
    ```bash
    .\huffman -c archivo.txt archivo.huff
    ```
*   **Descompresion**:
    ```bash
    .\huffman -d archivo.huff archivo_recuperado.txt 
    ```


