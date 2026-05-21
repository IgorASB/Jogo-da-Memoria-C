# ============================================================
#  Makefile — Jogo-da-Memoria-C
#  Uso: make         -> compila
#       make run     -> compila e executa
#       make clean   -> remove binários e arquivos gerados
# ============================================================

CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
TARGET = jogo_memoria
SRC    = src/jogo_memoria.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)
	@echo "[OK] Compilado com sucesso -> ./$(TARGET)"

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET) recordes.txt
	@echo "[OK] Binário e recordes removidos."

.PHONY: all run clean
