CC      := gcc
CFLAGS  := -fopenmp -Wall -Wextra -g -O2 -Iinclude
LDLIBS  := -lm

SRCDIR  := src
OBJDIR  := obj

# Recherche récursive de tous les .c dans src/
SRCS    := $(shell find $(SRCDIR) -type f -name "*.c")

# Transformation des chemins src/xyz.c -> obj/xyz.o
OBJS    := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

# Les cibles correspondantes : obj/foo.o -> foo
TARGETS := $(patsubst $(SRCDIR)/%.c, %, $(SRCS))

all: $(TARGETS)

# Règle pour compiler un exécutable à partir de son fichier .o principal
%: $(OBJDIR)/%.o
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)
	@echo "--- Exécutable $@ créé ---"

# Règle générique pour les objets (crée automatiquement les sous-dossiers dans obj/)
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGETS)

.PHONY: all clean
