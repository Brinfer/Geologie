#################################################################################
#																				#
# 							Organisation des sources							#
#																				#
#################################################################################

export BIN_DIR = bin
export SRC_DIR = src
export TST_DIR = test
export REPORT_DIR = coverage
export PROD = $(BIN_DIR)/geologie.out
export TEST = $(BIN_DIR)/geologie_test.out

SUB_DIR = $(TST_DIR) $(SRC_DIR)

#################################################################################
#																				#
# 							Regles du compilateur								#
#																				#
#################################################################################

# Options de compilation
export CCFLAGS += -O0
export CCFLAGS += -g -DDEBUG 		# avec debuggage : -g -DDEBUG # sans debuggage : -DNDEBUG
export CCFLAGS += -MMD -MP 			# gestion automatique des dépendances
export CCFLAGS += -D_BSD_SOURCE -D_XOPEN_SOURCE_EXTENDED -D_XOPEN_SOURCE -D_DEFAULT_SOURCE -D_GNU_SOURCE
export CCFLAGS += -std=c99 -Wall -pedantic
export CCFLAGS += -D_REENTRANT					# define a macro
export CCFLAGS += -lm							# Include math library
export CCFLAGS += -pthread -lpthread			# Include pthread library

all : build

# Compilation
build:
	[ -d $(BIN_DIR) ] || mkdir -p $(BIN_DIR)
	@for i in $(SUB_DIR); do (cd $$i; make $@); done

.PHONY: clean

# Nettoyage
clean:
	@for i in $(SRC_DIR); do (cd $$i; make $@); done
	@rm -f $(BIN_DIR)/*

update: clean build

# Execute le code
run: build
	$(PROD)

run_test: build
	$(TEST)

update: clean build

# Gestion d'un rapport de couverture de code par les tests
test_report:
	[ -d $(REPORT_DIR) ] || mkdir -p $(REPORT_DIR)
	@cd $(TST_DIR) && $(MAKE) $@

# Nettoyage du rapport de couverture
test_report_clean:
	@cd $(TST_DIR) && $(MAKE) $@
