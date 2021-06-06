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

# Packages
PACKAGES = PostmanLOG DispatcherLOG ProxyGUI ProxyLoggerMOB TranslatorLOG

#################################################################################
#																				#
# 							Regles du Makefile 		.							#
#																				#
#################################################################################

all: prod

# Compilation
prod: $(OBJ)
	@for p in $(PACKAGES); do (cd $$p && $(MAKE) $@); done

.c.o:
	$(CC) -c $(CCFLAGS) $< -o $@

# Nettoyage
.PHONY: clean

clean:
	@for p in $(PACKAGES); do (cd $$p && $(MAKE) $@); done
	@rm -f $(OBJ) $(DEP)

-include $(DEP)
