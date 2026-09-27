# --- CONFIGURATION DU PROJET ---
# Nom de l'exécutable final (sera program.exe sous Windows)
TARGET = main.exe

# Dossiers du projet
SRC_DIR = SRC
OBJ_DIR = obj

# Compilateur et options de compilation
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude -lm

# --- DÉTECTION DES FICHIERS SOURCE ---
# Utilise $(wildcard) pour lister les fichiers .c (évite le bug de la commande find)
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Génère la liste des fichiers .o correspondants dans le dossier obj
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# --- RÈGLES DE COMPILATION ---
# Règle principale (par défaut)
all: $(TARGET)

# Liaison de l'exécutable final
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Compilation des fichiers .c en fichiers .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Création du dossier obj si nécessaire (syntaxe Windows)
$(OBJ_DIR):
	@if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"

# Nettoyage du projet (supprime l'exécutable et les .o)
clean:
	@if exist "$(OBJ_DIR)" rmdir /s /q "$(OBJ_DIR)"
	@if exist "$(TARGET)" del /q "$(TARGET)"

# Indique à make que ces cibles ne sont pas des fichiers physiques
.PHONY: all clean
