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

all: prod

# Compilation
prod:
	@for p in $(PACKAGES); do (cd $$p && $(MAKE) $@); done


# Nettoyage
.PHONY: clean

clean:
	@for p in $(PACKAGES); do (cd $$p && $(MAKE) $@); done

-include $(DEP)
