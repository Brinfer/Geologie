# options de compilation
CCFLAGS += -O0
CCFLAGS += -g -DDEBUG # avec debuggage : -g -DDEBUG # sans debuggage : -DNDEBUG
CCFLAGS += -MMD -MP # gestion automatique des dépendances
CCFLAGS += -D_BSD_SOURCE -D_XOPEN_SOURCE_EXTENDED -D_XOPEN_SOURCE -D_DEFAULT_SOURCE -D_GNU_SOURCE
CCFLAGS += -std=c99 -Wall -pedantic
CCFLAGS += -lpthread         # Include pthread library
CCFLAGS += -D_REENTRANT     # define a macro

SRC_2 = serveurWifi.c

EXEC_2 = $(SRC_2:.c=.out)


# Compilation.
all: $(EXEC_2)

$(EXEC_2):
	$(CC) $(SRC_2) $(CCFLAGS) -o $(EXEC_2)

# Nettoyage.
.PHONY: clean

clean:
	@rm -f $(EXEC_2)
