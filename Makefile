# --- CONFIGURATION DU PROJET ---
TARGET = main.exe

# Dossiers du projet (Corrigé : src en minuscules)
SRC_DIR = src
OBJ_DIR = obj

# Compilateur et options de compilation
CC = gcc
# Note : Il est recommandé de mettre -lm à la fin de la ligne de liaison, 
# mais laissons les CFLAGS propres. Ajout de -Isrc car tes .h sont dans src/
CFLAGS = -Wall -Wextra -std=c11 -Isrc

# --- DÉTECTION DES FICHIERS SOURCE ---
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# --- RÈGLES DE COMPILATION ---
all: $(TARGET)

# Liaison de l'exécutable final (Ajout de -lm à la fin pour Linux)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lssl -lcrypto -lm

# Compilation des fichiers .c en fichiers .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Création du dossier obj si nécessaire (Syntaxe Linux)
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Nettoyage du projet (Syntaxe Linux)
clean:
	rm -rf $(OBJ_DIR)
	rm -f $(TARGET)

.PHONY: all clean
