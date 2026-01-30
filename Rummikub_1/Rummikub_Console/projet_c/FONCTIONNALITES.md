# Fonctionnalités Implémentées - Jeu Rummikub

## ✅ Règles Complètes Implémentées

### 1. Éléments du Jeu
- ✅ 104 tuiles numérotées (1-13) en 4 couleurs (rouge, bleu, noir, jaune), chaque tuile en double
- ✅ 2 tuiles joker
- ✅ Pioche complète avec mélange
- ✅ Chevalet par joueur (14 tuiles)
- ✅ Plateau de jeu pour les combinaisons

### 2. Début de Partie
- ✅ Mélange de toutes les tuiles
- ✅ Distribution de 14 tuiles par joueur
- ✅ Détermination du premier joueur (celui qui pioche la plus grande valeur)
- ✅ Remise des tuiles piochées dans la pioche après détermination

### 3. Combinaisons Autorisées
- ✅ **Suites (séquences)** : Suite de chiffres consécutifs de même couleur (minimum 3 tuiles)
- ✅ **Séries** : Mêmes chiffres de couleurs différentes (minimum 3 tuiles, maximum 4)
- ✅ Validation complète des combinaisons
- ✅ Gestion des jokers dans les combinaisons

### 4. Premier Coup (Règle des 30 Points)
- ✅ Vérification obligatoire : au moins 30 points pour le premier coup
- ✅ Si impossible, le joueur doit piocher et passer son tour
- ✅ Calcul automatique des points des combinaisons posées

### 5. Actions Pendant un Tour
Toutes les actions suivantes sont implémentées :

1. ✅ **Poser une nouvelle combinaison** depuis le chevalet
2. ✅ **Ajouter une tuile** à une combinaison existante (début, fin, ou pour compléter une série)
3. ✅ **Retirer une tuile** d'une combinaison (si > 3 tuiles) et la réutiliser immédiatement
4. ✅ **Diviser une suite** existante en deux suites valides
5. ✅ **Remplacer une tuile** dans une combinaison (en maintenant la validité)
6. ✅ **Piocher une tuile** si aucune action n'est possible

### 6. Gestion des Jokers
- ✅ Le joker peut remplacer n'importe quelle tuile dans une combinaison
- ✅ Valeur du joker = valeur de la tuile qu'il remplace
- ✅ **Récupération de joker** : un joueur peut récupérer un joker du plateau en le remplaçant par la tuile exacte correspondante de son chevalet
- ✅ Le joker récupéré doit être rejoué immédiatement
- ✅ **Interdiction** : impossible de diviser une combinaison contenant un joker
- ✅ **Fin de partie** : joker sur le chevalet = 30 points négatifs

### 7. Validation du Plateau
- ✅ Vérification que toutes les combinaisons restent valides après chaque action
- ✅ Si une action rend le plateau invalide, elle est annulée automatiquement
- ✅ Sauvegarde/restauration du plateau pour annuler les actions invalides

### 8. Joueur Ordinateur (IA)
- ✅ Mode humain contre ordinateur disponible
- ✅ Stratégie IA implémentée :
  - Recherche de toutes les combinaisons possibles
  - Évaluation des coups possibles
  - Choix du meilleur coup (poser combinaison, ajouter tuile, récupérer joker)
  - Gestion du premier coup (30 points)
- ✅ Si aucun coup possible, l'IA pioche automatiquement

### 9. Fin de Partie et Calcul des Scores
- ✅ **Victoire normale** : un joueur pose sa dernière tuile → il gagne
- ✅ **Victoire par points** : si la pioche est vide, le joueur avec le moins de points gagne
- ✅ **Calcul des scores** :
  - Joueurs perdants : somme négative des valeurs des tuiles restantes (joker = 30 points négatifs)
  - Gagnant : somme de tous les scores négatifs des autres joueurs en points positifs
- ✅ Sauvegarde automatique des scores dans un fichier

### 10. Variante Optionnelle (Timer)
- ✅ Option disponible au démarrage pour activer le timer
- ⚠️ Interface prête, implémentation complète à finaliser (gestion du temps écoulé et pénalités)

## 📁 Structure des Fichiers

### Fichiers Principaux
- `main_console.c` : Programme principal avec interface console complète
- `structures.h/c` : Définitions des structures de données
- `jeu.h/c` : Fonctions de base du jeu (pioche, distribution, validation)
- `plateau.h/c` : Gestion du plateau et des combinaisons
- `ia.h/c` : Intelligence artificielle pour joueur ordinateur
- `fichiers.h/c` : Sauvegarde des pseudos et scores

### Fonctions Clés

#### Validation
- `est_suite_valide()` : Valide une suite
- `est_serie_valide()` : Valide une série
- `est_combinaison_valide()` : Valide une combinaison (suite ou série)
- `plateau_est_valide()` : Vérifie que tout le plateau est valide

#### Gestion du Plateau
- `ajouter_combinaison_plateau()` : Ajoute une combinaison
- `ajouter_tuile_combinaison()` : Ajoute une tuile à une combinaison
- `retirer_tuile_combinaison()` : Retire une tuile d'une combinaison
- `diviser_combinaison()` : Divise une suite en deux
- `remplacer_tuile_combinaison()` : Remplace une tuile
- `recuperer_joker()` : Récupère un joker du plateau

#### IA
- `choisir_coup_ia()` : Choisit le meilleur coup pour l'IA
- `trouver_combinaisons_possibles_ia()` : Trouve toutes les combinaisons possibles
- `evaluer_coup()` : Évalue un coup possible

## 🎮 Utilisation

### Compilation
```bash
gcc structures.c jeu.c fichiers.c plateau.c ia.c main_console.c -o rummikub_console
```

### Exécution
```bash
./rummikub_console
```

### Options au Démarrage
1. Nombre de joueurs (2-4)
2. Activation de la variante timer (optionnel)
3. Configuration de chaque joueur :
   - Pseudo
   - Type (Humain ou IA)

### Actions Disponibles
Le menu complet propose toutes les actions possibles selon les règles officielles du Rummikub.

## 📝 Notes d'Implémentation

- Toutes les règles officielles du Rummikub sont respectées
- Le code est modulaire et bien structuré
- Gestion d'erreurs complète pour les actions invalides
- Interface console claire et informative
- Sauvegarde automatique des pseudos et scores

## 🔄 Améliorations Futures Possibles

1. Interface graphique (SDL2 ou autre)
2. Implémentation complète du timer avec pénalités
3. Amélioration de la stratégie IA (plus élaborée)
4. Mode multijoueur en réseau
5. Historique des parties
6. Statistiques des joueurs
