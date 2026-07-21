CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
SRC_DIR = src
RES_DIR = resultados
TARGET = huffman
TEST_TARGET = test_runner

ifeq ($(OS),Windows_NT)
    RUN_CMD = .\$(TARGET)
    RUN_TEST = .\$(TEST_TARGET)
else
    RUN_CMD = ./$(TARGET)
    RUN_TEST = ./$(TEST_TARGET)
endif

# Archivos fuente
OBJ = $(SRC_DIR)/main.o $(SRC_DIR)/huffman_tree.o $(SRC_DIR)/priority_queue.o $(SRC_DIR)/compressor.o
TEST_OBJ = $(SRC_DIR)/test_runner.o $(SRC_DIR)/huffman_tree.o $(SRC_DIR)/priority_queue.o $(SRC_DIR)/compressor.o

# Compilar todo
all: $(TARGET) $(TEST_TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) -lm

$(TEST_TARGET): $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_OBJ) -lm

$(SRC_DIR)/main.o: $(SRC_DIR)/main.c $(SRC_DIR)/compressor.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_DIR)/test_runner.o: $(SRC_DIR)/test_runner.c $(SRC_DIR)/compressor.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_DIR)/huffman_tree.o: $(SRC_DIR)/huffman_tree.c $(SRC_DIR)/huffman_tree.h $(SRC_DIR)/priority_queue.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_DIR)/priority_queue.o: $(SRC_DIR)/priority_queue.c $(SRC_DIR)/priority_queue.h $(SRC_DIR)/huffman_tree.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_DIR)/compressor.o: $(SRC_DIR)/compressor.c $(SRC_DIR)/compressor.h $(SRC_DIR)/huffman_tree.h $(SRC_DIR)/priority_queue.h
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar archivos (compatible con Windows y Linux)
clean:
ifeq ($(OS),Windows_NT)
	-cmd /c "del /q /f src\*.o $(TARGET).exe $(TEST_TARGET).exe $(TARGET) $(TEST_TARGET) 2>nul"
	-cmd /c "del /q /f $(RES_DIR)\* 2>nul"
else
	rm -f $(SRC_DIR)/*.o $(TARGET) $(TEST_TARGET)
	rm -rf $(RES_DIR)/*
endif

# Comprimir archivo (detecta archivo.txt o texto.txt)
compress: $(TARGET)
ifeq ($(OS),Windows_NT)
	@if exist archivo.txt ( $(RUN_CMD) -c archivo.txt $(RES_DIR)/archivo.huff ) else ( $(RUN_CMD) -c texto.txt $(RES_DIR)/texto.huff )
else
	@if [ -f archivo.txt ]; then $(RUN_CMD) -c archivo.txt $(RES_DIR)/archivo.huff; else $(RUN_CMD) -c texto.txt $(RES_DIR)/texto.huff; fi
endif

# Descomprimir archivo
decompress: $(TARGET)
ifeq ($(OS),Windows_NT)
	@if exist $(RES_DIR)\archivo.huff ( $(RUN_CMD) -d $(RES_DIR)/archivo.huff $(RES_DIR)/archivo_recuperado.txt ) else ( $(RUN_CMD) -d $(RES_DIR)/texto.huff $(RES_DIR)/texto_recuperado.txt )
else
	@if [ -f $(RES_DIR)/archivo.huff ]; then $(RUN_CMD) -d $(RES_DIR)/archivo.huff $(RES_DIR)/archivo_recuperado.txt; else $(RUN_CMD) -d $(RES_DIR)/texto.huff $(RES_DIR)/texto_recuperado.txt; fi
endif

# Ejecutar pruebas automatizadas
test: all
	$(RUN_TEST)

run: compress decompress

