#################################################################################
#																				#
# 							Organisation des sources							#
#																				#
#################################################################################

# Packages
PACKAGES = PostmanLOG DispatcherLOG ProxyGUI ProxyLoggerMOB TranslatorLOG

#################################################################################
#																				#
# 							Regles du Makefile 		.							#
#																				#
#################################################################################

all: test

# Compilation
test: $(OBJ)
	@for p in $(PACKAGES); do (cd $$p && $(MAKE) $@); done

# Nettoyage
.PHONY: clean

clean:
	@for p in $(PACKAGES); do (cd $$p && $(MAKE) $@); done

