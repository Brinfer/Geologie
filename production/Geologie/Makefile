#################################################################################
#																				#
# 							Organisation des sources							#
#																				#
#################################################################################

export BIN_DIR = bin
export SRC_DIR = src
export TST_DIR = test
export COVERAGE_DIR = coverage
export CHECK_DIR = check
export DOC_DIR = doc
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
export CCFLAGS += -g -DDEBUG # avec debuggage : -g -DDEBUG # sans debuggage : -DNDEBUG
export CCFLAGS += -MMD -MP # gestion automatique des dpendances
export CCFLAGS += -D_BSD_SOURCE -D_XOPEN_SOURCE_EXTENDED -D_XOPEN_SOURCE -D_DEFAULT_SOURCE -D_GNU_SOURCE
export CCFLAGS += -std=c99 -Wall -pedantic
export CCFLAGS += -D_REENTRANT					# define a macro

export LDFLAGS += -lm							# Include math library
export LDFLAGS += -pthread -lpthread			# Include pthread library
export LDFLAGS += -lbluetooth					# Include bluetooth library
export LDFLAGS += -lrt					        # Include mqueue library

all : prod test

# Compilation
prod:
	[ -d $(BIN_DIR) ] || mkdir -p $(BIN_DIR)
	@cd $(SRC_DIR) && $(MAKE) $@

test: prod
	[ -d $(BIN_DIR) ] || mkdir -p $(BIN_DIR)
	@cd $(TST_DIR) && $(MAKE) $@

check:
	[ -d $(CHECK_DIR) ] || mkdir -p $(CHECK_DIR)
	cppcheck --enable=all --xml --xml-version=2 ./ 2>$(CHECK_DIR)/report.xml
	cppcheck-htmlreport --source-dir=./ --file=$(CHECK_DIR)/report.xml --report-dir=$(CHECK_DIR) --title=GEOLOGIE

# Gestion d'un rapport de couverture de code par les tests
coverage:
	[ -d $(COVERAGE_DIR) ] || mkdir -p $(COVERAGE_DIR)
	@cd $(TST_DIR) && $(MAKE) $@

doc:
	doxygen Doxyfile

# Nettoyage
clean:
	@for i in $(SUB_DIR); do (cd $$i; make $@); done
	@rm -rf $(BIN_DIR)/

check_clean:
	@rm -rf $(CHECK_DIR)/

# Nettoyage du rapport de couverture
coverage_clean:
	@cd $(TST_DIR) && $(MAKE) $@
	@rm -rf $(COVERAGE_DIR)/

doc_clean:
	rm -rf $(DOC_DIR)/

clean_all : clean coverage_clean doc_clean check_clean

# Execute le code
run: prod
	$(PROD)

run_test: test
	$(TEST)

update: clean prod test

help:
	@echo -e "\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Projet ProSE A1 ST <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n"
	@echo -e "Régles disponibles:\n"

	@echo -e "\t#################################################### Compilation ####################################################\n"
	@echo -e "\tprod\t\t\t Compile et génére l'éxécutable de Géologie dans le  dossier \033[34mbin\033[0m"
	@echo -e "\ttest\t\t\t Compile et génére l'éxécutable de des tests de Géologie"
	@echo -e "\tcheck\t\t\t Exécute une analyse statique sur le code source et export le résultat dans le dossier \033[34mreport\033[0m"
	@echo -e "\tcoverage\t\t Génére un rapport de couverture des tests dans le dossier \033[34mcoverage\033[0m"
	@echo -e "\tdoc\t\t\t Génére la documentation du code source dans le dossier \033[34mdoc\033[0m"

	@echo -e "\n\t##################################################### Nettoyage #####################################################\n"
	@echo -e "\tclean\t\t\t Supprime les fichiers généré par les commandes \033[33mprod\033[0m et \033[33mtest\033[0m"
	@echo -e "\tcheck_clean\t\t Supprime les fichiers généré par la commande \033[33mcheck\033[0m"
	@echo -e "\tcoverage_clean\t\t Supprime les fichiers généré par la commande \033[33mcoverage\033[0m"
	@echo -e "\tdoc_clean\t\t Supprime les fichiers généré par la commande \033[33mdoc\033[0m"
	@echo -e "\tclean_all\t\t Exécute les commandes (dans l'ordre) \033[33mclean\033[0m, \033[33mcheck_clean\033[0m, \033[33mcoverage_clean\033[0m et \033[33mdoc_clean\033[0m"

	@echo -e "\n\t##################################################### Exécution #####################################################\n"
	@echo -e "\trun\t\t\t Lance l'éxécutable généré par la commande \033[33mprod\033[0m"
	@echo -e "\trun_test\t\t Lance l'éxécutable généré par la commande \033[33mtest\033[0m"

	@echo -e "\n\t####################################################### Autre #######################################################\n"
	@echo -e "\tupdate\t\t\t Exécute les commandes (dans l'ordre) \033[33mclean\033[0m \033[33mprod\033[0m et \033[33mtest\033[0m"
	@echo -e "\tall\t\t\t Exécute les commandes (dans l'ordre) \033[33mprod\033[0m et \033[33mtest\033[0m"
	@echo -e "\thelp\t\t\t Affiche cet écran"

	@echo -e "\n\n\tPour compiler en utilisant le compilateur pour la carte STM32MP1 entrer la commande:"
	@echo -e "\n\t\t\033[32msource ../../../SDK/environment-setup-cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi\033[0m\n"
	@echo -e "\tPenser à nettoyer les anciens fichiers compilés avant de recompiler (voir la commande \033[33mclean\033[0m)."

	@echo -e "\n\n\tPour evpyer le fichier compiler à la carte STM32MP1 entrer la commande:"
	@echo -e "\n\t\t\033[32mscp bin/geologie.out root@<Ip>:/home/root/geologie.out\033[0m\n"

	@echo -e "\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Projet ProSE A1 ST <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n"
.PHONY: clean
.PHONY: test
.PHONY: check
.PHONY: doc
