#################################################################################
#																				#
# 							Organisation des sources							#
#																				#
#################################################################################

# Packages.
PACKAGES = Geographer ManagerLOG UI Mathematician Scanner CommGeologie Led

SRC = $(wildcard */*.c) $(wildcard */**/*.c)
OBJ = $(SRC:.c=.o)

# Point d'entre du programme
MAIN = main.c

# Gestion automatique des dpendances
DEP = $(MAIN:.c=.d)

# Executable à gnrer.
EXEC = ../$(PROD)

# Inclusion depuis le niveau du package
CCFLAGS += -I.

#################################################################################
#																				#
# 							Regles du Makefile 		.							#
#																				#
#################################################################################

all: prod

# Compilation
prod:
	@for p in $(PACKAGES); do (cd $$p && $(MAKE) $@); done
	$(MAKE) $(EXEC)

$(EXEC): $(OBJ) $(MAIN)
	$(CC) $(CCFLAGS) $(OBJ) $(MAIN) -MF $(DEP) -o $(EXEC) $(LDFLAGS)

# Nettoyage
clean:
	@for p in $(PACKAGES); do (cd $$p && $(MAKE) $@); done
	@rm -f $(DEP) $(OBJ)

-include $(DEP)


.PHONY: clean
