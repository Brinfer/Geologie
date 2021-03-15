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

## Règle sur le Git

Pour plus de détails ou d'information, il y de nombreuses ressources sur [internet](https://www.atlassian.com/fr/git/tutorials/learn-git-with-bitbucket-cloud)

### Les branches

Créer une branche signifie diverger de la ligne principale de développement et continuer à travailler sans impacter cette ligne.

Il est important de ne pas faire de modification sur la branche principale: _master_. Celle-ci ne doit être modifier que au travers de _merge_ ou de _pull request_. Vous devez donc créer des _branch_.

Il y a certaines règles à respecter sur le nommage des branches:
* Mettre le numéro de la tâche associer à ce que vous à la branches
* Préciser ce que vous y faites

Structure: ```#<numTache>: <MessagePrecis>```

#### Commandes

Créer du branche:
```bash
git branch "<nomDeLaBranche>"
```
_(La branche n'existe que en locale, elle n'est toujours présente sur le dépot distant [voir la partie sur la dépot distant](#le-dépot-distant), pensez à respecter la structure des nom des [branches](#les-branches))_

&nbsp;

Lister les branches en locale:
```bash
git branch
```

&nbsp;

Lister les branches distante:
```bash
git branch -r
```

&nbsp;

Changer de branche:
```bash
git checkout "<nomDeMaBranche>"
```

&nbsp;

Fusionner une branche vers celle où je me trouve:
```bash
git merge "<nomDeLaBrancheAFusionner>"
```
_(Des conflits peuvent apparaître, c'est alors à vous de les gérer. Le mieux est alors de vous référer à la documentation de l'IDE que vous utilisez car celui-ci peut vous aidez à les visualiser. Pour plus de détails sur les conflits: [lien](https://www.atlassian.com/fr/git/tutorials/using-branches/merge-conflicts))_

&nbsp;

Supprimer une branches en locale:
```bash
git branch -d "<nomDeLaBranche>"
```
_(La branche n'est supprimer que en locale, elle est toujours présente sur le dépot distant, [voir la partie sur la dépot distant](#le-dépot-distant))_

&nbsp;

Renommer une branche en locale:
```bash
git branch -m "<nouveauNom>"
```
_(La branche n'est renommer que en locale, elle à toujours sont ancien nom sur le dépot distant, [voir la partie sur la dépot distant](#le-dépot-distant), pensez à respecter la structure des noms des [branches](#les-branches))_


### Les Commits

Un _commit_ permet de valider les modifications apportées. Notez que tout _commit_ ne sere pas dans le dépôt distant tant qu'il n'est pas pousser ([voir la partie sur la dépot distant](#le-dépot-distant)).

Il y a certaines règles à respecter sur les messages des commits:
* Mettre le numéro de la tâche associer à ce que vous faites
* Préciser ce que vous y faites

Structure: ```#<numTache>: <MessagePrecis>```

#### Commandes

Dans un premier temps il faut ajouter les fichiers modifiers ou ajouté à l'index, pour cela il faut utiliser la commande _add_:
```bash
git add "chemin/vers/mon/fichier"
```
_(Pour ajouter toutes les modifications du projet, un ```git add .``` suffit)_

Il faut ensuite valider les changements:
```bash
git commit -m "<monMessage>"
```
_(Le commit n'existe que en locale, il n'est toujours présente sur le dépot distant [voir la partie sur la dépot distant](#le-dépot-distant), pensez à respecter la structure des nom des [commits](#les-commits))_

&nbsp;

Pour voir les précédents _commit_:
```bash
git log
```
_(Pour arrêter de visualiser appuyer sur la touche __q__)_

&nbsp;

Pour modifier le dernier _commit_ (faites le avant de pousser le commit):
```bash
git commit --amend
```
Un fichier sera ouvert et vous pourrez modifier l'ensemble des informations du commit. Pour plus de détail voir la [documentation](https://www.atlassian.com/fr/git/tutorials/rewriting-history).
Pour juste modifier le message du commit:
```bash
git commit --amend -m "<nouveauMessage>"
```
_(Pensez à respecter la structure des [commits](#les-commits))_

&nbsp;

De nombreuses autres commandes existes et permettent de faire de nombreuse chose. Je vous invite à vous renseigner de vous même dessus en fonction de vos besoins.

### Le dépot distant

Un dépôt distant est une version du projet qui est hébergée sur un serveur. Collaborer avec d’autres personnes consiste à gérer ce dépôt distant, en poussant ou tirant des données depuis et vers ces dépôts quand vous souhaitez partager votre travail.

#### Commandes

Pousser vos modification locale vers le dépot distant:
```bash
git push
```

&nbsp;

Tirer les modification du dépot distant:
```bash
git pull
```

&nbsp;

Pousser une nouvelle branches ou une branche renommé:
```bash
git push -u "<leNomDeMaBranche>"
```

&nbsp;

Supprimer une branches du dépot distant:
```bash
git push <nomDuDepot> --delete "#6666: Ma nouvelle branche / Ma branche renommé"
```
_(le ```nomDuDepot``` est généralement __origin__)_

&nbsp;

Sauvegarder les mots de passe:
```bash
git config --local credential.helper store
```
Il suffit de pousser ou de tirer ensuite (l'idée est d'entrer de nouveau vôtre mot de passe). Les identifiants sont lors sauvegarder et automatiquement réutilisés. __Attention__ ceux-ci sont sauvegardé dans un fichier texte sans aucun encodage, vos identifiant sont visisible en clair ce qui est un défaut de sécurité.

Vous pouvez aussi sauvegardé ces identifiant de manière temporaire:
```bash
git config --local credential.helper cache --timeout <seconde>
```
Par défaut, ```seconde``` vaut 900 soit 15 minutes.
