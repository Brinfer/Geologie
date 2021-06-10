#################################################################################
#																				#
# 							Organisation des sources							#
#																				#
#################################################################################

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
DEP = $(SRC:.c=.d)

# Inclusion depuis le niveau du package.
CCFLAGS += -I..

#################################################################################
#																				#
# 							Regles du Makefile 		.							#
#																				#
#################################################################################

all: prod

# Compilation
prod: $(OBJ)

.c.o:
	$(CC) -c $(CCFLAGS) $< -o $@

# Nettoyage
.PHONY: clean

clean:
	@rm -f $(OBJ) $(DEP)

-include $(DEP)
