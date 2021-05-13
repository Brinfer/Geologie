# README

Dans tout ce __README__ nous concéderons que le terminal est placé au niveau du dossier du projet.

```bash
cd path/vers/le/dossier/du/projet
```

## Installation

Après le clonage du dépot, pensez à executer cette commande:

```bash
git config core.hooksPath .githooks
```

Et ce afin que tout le monde utilise les même [git hook](https://www.atlassian.com/git/tutorials/git-hooks).

### Architecture du projet

Dans ce projet, le [framework cmocka](https://cmocka.org/) est utilisé. Celui-ci doit se trouver dans un dossier nommé `cmocka-1.1.5_x86_64`. La structure du projet doit donc être (en se plaçant un dossier au dessus de la racine du projet (`cd ..`)):

```bash
.
├──cmocka-1.1.5_x86_64
|  └── ...
├── SDK
|  └── ...
└── se2022-a1.c
   ├── explorations
   ├── production
   ├── template
   └── README.md
```

#### exploration

Ce dossier contient les codes élaborés lors d'exploration, ceux-ci peuvent être réutilisé pour le code de production.

#### production

Ce dossier contient le code de production, celui composant l'exécutable finale.

#### template

Ce dossier contient deux fichiers d'exemples et un dossier de configuration:

```bash
.
├── code template
│  ├── C_Header_File.xml
│  ├── C_Source_File.xml
│  ├── File_Comment.xml
│  └── Formater.xml
├── example.c
└── example.h
```

Les fichiers _exemple.h_ et _exemple.c_ décrivent rapidement les règles de codage pour le dossiers de [production](#production).

Le dossier _code template_ contient des fichiers de configuration de l'[environnement de développement](#st-cube-ide).

*C_Header_File.xml_* et *C_Source_File.xml* sont des fichiers de configuration des templates des fichiers sources et headers. *Formater.xml* est le fichier de configuration du formater des fichiers.

### ST CUBE IDE

Pour l'installation de l'environnement de développement référer vous à la documentation du [IDE fourni par ST](https://www.st.com/en/development-tools/stm32cubeide.html#documentation).

Après l'installation il faut adapter certains paramètres (template des fichiers sources et headers ainsi que celui du formateurs, voir [dossier template](#template)). Voir la page [Code Templates Preferences](https://www.eclipse.org/pdt/help/html/code_templates_preferences.htm) et [Import XML file for path and symbols programmatically](https://www.eclipse.org/forums/index.php/t/1096273/)

### Cross-compilation

La cible du projet est une _STM32MP1_ utilisant comme OS _OpenStLinux 5.10_. Cet OS utilise un compilateur spécifique __ostl-linux-gnueabi__.
Afin de pouvoir facilement il faut changer la valeur de la variable __CC__ (on concidere ici que le dossier contenant le compilateur ce trouve au-dessus de la racine du projet, voir [section Architecture du projet](#architecture-du-projet)).

```bash
source ../SDK/environment-setup-cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi
```

Pour tester le bon changement il faut utiliser la commande:

```bash
$CC --version
```

et doit retourner:

```bash
arm-ostl-linux-gnueabi-gcc (GCC) 9.3.0
Copyright (C) 2019 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```
