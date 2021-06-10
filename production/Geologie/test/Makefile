#################################################################################
#																				#
# 							Organisation des sources							#
#																				#
#################################################################################

# Packages.
PACKAGES = Geographer ManagerLOG UI Mathematician Scanner CommGeologie Led TranslatorBeacon

#################################################################################
#																				#
# 							Organisation librairies 							#
#																				#
#################################################################################

# Framework de test CMocka.
CMOCKA = $(realpath ../../../../cmocka-1.1.5_x86_64/)

# Couverture du code
export GCOVR = gcovr

#################################################################################
#																				#
# 							Regles du compilateur								#
#																				#
#################################################################################

# Wrap des mthodes du Stock pour bouchonage.
#exemple LDWRAP  = -Wl,--wrap=<symbol>
LDWRAP =

# Inclusion depuis le niveau du package.
CCFLAGS += -I$(CMOCKA)/include

# Liens pour les tests.
LDFLAGS += -L$(CMOCKA)/lib

# Utilisation de la librairie static CMocka
LDFLAGS += -lcmocka-static
# Force l'utilisation d'une librairie statique pour CMocka.
# LDFLAGS += -Wl,-static -lcmocka-static -Wl,-Bdynamic

# Gcov informations
CCFLAGS += -fprofile-arcs -ftest-coverage -fprofile-abs-path

#################################################################################
#																				#
# 							Organisation des sources							#
#																				#
#################################################################################

SRC  = $(wildcard */*.c) $(wildcard */**/*.c)
OBJ = $(SRC:.c=.o)

# Point d'entre du programme
MAIN = main_test.c

# Gestion automatique des dpendances
DEP = $(MAIN:.c=.d)

# Executable à gnrer.
EXEC = ../$(TEST)

# Inclusion depuis le niveau du package
CCFLAGS += -I.

GCDA = $(MAIN:.c=.gcda)
GCNO = $(MAIN:.c=.gcno)

GCOVR_DIR = ../$(COVERAGE_DIR)/

#################################################################################
#																				#
# 							Regles du Makefile 		.							#
#																				#
#################################################################################

all: test

# Compilation
test:
	@for p in $(PACKAGES); do (cd $$p && $(MAKE) $@); done
	$(MAKE) $(EXEC)

$(EXEC): $(OBJ) $(MAIN)
	$(CC) $(CCFLAGS) $(LDWRAP) $(realpath $(OBJ)) $(MAIN) -MF $(DEP) -o $(EXEC) $(LDFLAGS)

# Generation du rapport de couverture
coverage:
	$(GCOVR) -r .. --html --html-details -o $(GCOVR_DIR)/index.html

# Nettoyage
clean: coverage_clean
	@for p in $(PACKAGES); do (cd $$p && $(MAKE) $@); done
	@rm -f $(DEP) $(GCDA) $(GCNO)

# Nettoyage du rapport de couverture
coverage_clean:
	@rm -rf $(GCOVR_DIR)/

-include $(DEP)


.PHONY: clean
.PHONY: test
