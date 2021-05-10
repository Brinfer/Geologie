#################################################################################
#																				#
# 							Organisation des sources							#
#																				#
#################################################################################

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
DEP = $(SRC:.c=.d)

# Gcov informations
GCDA = $(SRC:.c=.gcda)
GCNO = $(SRC:.c=.gcno)

# Inclusion depuis le niveau du package.
CCFLAGS += -I.. -I../../$(SRC_DIR)

#################################################################################
#																				#
# 							Regles du Makefile 		.							#
#																				#
#################################################################################

all: build

# Compilation
build: $(OBJ)

.c.o:
	$(CC) -c $(CCFLAGS) $< -o $@

# Nettoyage
.PHONY: clean

clean:
	@rm -f $(OBJ) $(DEP)

-include $(DEP)
