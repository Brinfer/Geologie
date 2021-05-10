#################################################################################
#																				#
# 							Organisation des sources							#
#																				#
#################################################################################

# Packages.
PACKAGES = Geographer \
	ManagerLOG \
	UI \
	ReceptionistLOG\

SRC  = $(wildcard */*.c)
OBJ = $(SRC:.c=.o)

# Point d'entrée du programme
MAIN = main.c

# Gestion automatique des dépendances
DEP = $(MAIN:.c=.d)

# Executable à générer.
EXEC = ../$(PROD)

# Inclusion depuis le niveau du package
CCFLAGS += -I.

#################################################################################
#																				#
# 							Regles du Makefile 		.							#
#																				#
#################################################################################

all: build

# Compilation
build:
	@for p in $(PACKAGES); do (cd $$p && $(MAKE) $@); done
	$(MAKE) $(EXEC)

$(EXEC): $(OBJ) $(MAIN)
	$(CC) $(CCFLAGS) $(OBJ) $(MAIN) -MF $(DEP) -o $(EXEC) $(LDFLAGS)

# Nettoyage
.PHONY: clean

clean:
	@for p in $(PACKAGES); do (cd $$p && $(MAKE) $@); done
	@rm -f $(DEP) $(OBJ)

-include $(DEP)
