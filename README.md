# Sujet de programmation impérative

La page du sujet :

https://www.labri.fr/perso/renault/working/teaching/projets/2022-23-S6-C-Amazons.php

La page sur thor :

https://thor.enseirb-matmeca.fr/ruby/projects/projetss6-amaz/overview

Ce fichier README fournit des informations sur le projet en C "Jeu des Amazones". Le projet est divisé en deux parties : le serveur et le client.

## Makefile

Le Makefile fournit les instructions pour compiler et exécuter le projet. Voici les options et les cibles disponibles :

- **all**: Compile et construit le serveur, le client et les tests.
- **build**: Construit le serveur et le client.
- **server**: Compile et construit le fichier exécutable du serveur.
- **client**: Compile les objets nécessaires au client.
- **alltests**: Compile et construit tous les tests.- **valgrind**: Exécute avec Valgrind pour détecter les fuites de mémoire.
- **test**: Exécute tous les tests.
- **install**: Installe le serveur, le client et exécute les tests.
- **run**: Exécute le serveur avec les clients installés.
- **run_print_grid**: Exécute le serveur avec les clients installés et affiche la grille.
- **pdf**: Génère le rapport en format PDF.
- **doc**: Génère la documentation avec Doxygen.
- **cleanTest**: Supprime les fichiers générés par les tests.
- **clean**: Supprime tous les fichiers générés.

## Options de la fonction getopt

La fonction getopt est utilisée pour récupérer les options d'exécution. Voici les options implémentées :

- **-m X**: Spécifie la largeur du plateau de jeu (par défaut: 8).
- **-t X**: Spécifie la forme du plateau de jeu (par défaut: 'c', sinon 'd' ou 'g').
- **-s X**: Spécifie la graine aléatoire pour la génération du plateau de jeu.
- **-p**: Active l'affichage de la grille du jeu.

Exemple d'utilisation: `./serveur ./nom_client1 ./nom_client2 -m 8 -t c -s 123 -p`

Note: Consultez le code source pour plus de détails sur la manière dont ces options sont utilisées dans le projet.

**Note**: Les chemins des dépendances sont spécifiques au système et peuvent nécessiter des ajustements pour votre environnement de développement. Veuillez les vérifier et les modifier si nécessaire.
