#include "plateau.h"
#include "jeu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Vérifier si une combinaison est valide (suite ou série)
int est_combinaison_valide(Combinaison *comb) {
    if (comb->nb_tuiles < 3) return 0;
    
    if (comb->type == 0) { // Suite
        return est_suite_valide(comb->tuiles, comb->nb_tuiles);
    } else { // Série
        return est_serie_valide(comb->tuiles, comb->nb_tuiles);
    }
}

// Calculer les points d'une combinaison
int calculer_points_combinaison_complete(Combinaison *comb) {
    int points = 0;
    for (int i = 0; i < comb->nb_tuiles; i++) {
        if (comb->tuiles[i].type == JOKER) {
            // Pour un joker dans une suite, sa valeur dépend de sa position
            if (comb->type == 0) { // Suite
                // Trouver la valeur minimale et maximale
                int min_val = 14, max_val = 0;
                for (int j = 0; j < comb->nb_tuiles; j++) {
                    if (comb->tuiles[j].type == NORMAL) {
                        if (comb->tuiles[j].valeur < min_val) min_val = comb->tuiles[j].valeur;
                        if (comb->tuiles[j].valeur > max_val) max_val = comb->tuiles[j].valeur;
                    }
                }
                // Estimer la valeur du joker (approximation)
                points += (min_val + max_val) / 2;
            } else { // Série
                // Dans une série, le joker a la même valeur que les autres tuiles
                for (int j = 0; j < comb->nb_tuiles; j++) {
                    if (comb->tuiles[j].type == NORMAL) {
                        points += comb->tuiles[j].valeur;
                        break;
                    }
                }
            }
        } else {
            points += comb->tuiles[i].valeur;
        }
    }
    return points;
}

// Vérifier si une combinaison contient un joker
int a_joker(Combinaison *comb) {
    for (int i = 0; i < comb->nb_tuiles; i++) {
        if (comb->tuiles[i].type == JOKER) {
            return 1;
        }
    }
    return 0;
}

// Ajouter une combinaison au plateau
int ajouter_combinaison_plateau(Plateau *plateau, Combinaison *comb) {
    if (!est_combinaison_valide(comb)) {
        return 0;
    }
    
    if (plateau->nb_combinaisons >= 50) {
        return 0; // Plateau plein
    }
    
    plateau->combinaisons[plateau->nb_combinaisons] = *comb;
    plateau->nb_combinaisons++;
    return 1;
}

// Retirer une combinaison du plateau
int retirer_combinaison_plateau(Plateau *plateau, int index) {
    if (index < 0 || index >= plateau->nb_combinaisons) {
        return 0;
    }
    
    // Décaler les combinaisons vers la gauche
    for (int i = index; i < plateau->nb_combinaisons - 1; i++) {
        plateau->combinaisons[i] = plateau->combinaisons[i + 1];
    }
    plateau->nb_combinaisons--;
    return 1;
}

// Modifier une combinaison sur le plateau
int modifier_combinaison_plateau(Plateau *plateau, int index, Combinaison *nouvelle_comb) {
    if (index < 0 || index >= plateau->nb_combinaisons) {
        return 0;
    }
    
    if (!est_combinaison_valide(nouvelle_comb)) {
        return 0;
    }
    
    plateau->combinaisons[index] = *nouvelle_comb;
    return 1;
}

// Diviser une suite en deux suites valides
int diviser_combinaison(Plateau *plateau, int index_comb, int index_division, Combinaison *comb1, Combinaison *comb2) {
    if (index_comb < 0 || index_comb >= plateau->nb_combinaisons) {
        return 0;
    }
    
    Combinaison *comb_originale = &plateau->combinaisons[index_comb];
    
    // Ne pas diviser une combinaison avec joker
    if (a_joker(comb_originale)) {
        return 0;
    }
    
    // Ne pas diviser si moins de 4 tuiles
    if (comb_originale->nb_tuiles < 4) {
        return 0;
    }
    
    // Ne peut diviser que des suites
    if (comb_originale->type != 0) {
        return 0;
    }
    
    if (index_division < 1 || index_division >= comb_originale->nb_tuiles - 1) {
        return 0;
    }
    
    // Créer la première combinaison
    comb1->nb_tuiles = index_division;
    comb1->type = 0; // Suite
    for (int i = 0; i < index_division; i++) {
        comb1->tuiles[i] = comb_originale->tuiles[i];
    }
    
    // Créer la deuxième combinaison
    comb2->nb_tuiles = comb_originale->nb_tuiles - index_division;
    comb2->type = 0; // Suite
    for (int i = 0; i < comb2->nb_tuiles; i++) {
        comb2->tuiles[i] = comb_originale->tuiles[index_division + i];
    }
    
    // Vérifier que les deux combinaisons sont valides
    if (!est_combinaison_valide(comb1) || !est_combinaison_valide(comb2)) {
        return 0;
    }
    
    return 1;
}

// Ajouter une tuile à une combinaison
int ajouter_tuile_combinaison(Combinaison *comb, Tuile tuile) {
    if (comb->nb_tuiles >= 13) {
        return 0; // Combinaison pleine
    }
    
    // Créer une nouvelle combinaison temporaire pour tester
    Combinaison comb_test = *comb;
    comb_test.tuiles[comb_test.nb_tuiles] = tuile;
    comb_test.nb_tuiles++;
    
    // Trier si nécessaire et vérifier la validité
    if (comb->type == 0) { // Suite - insérer au bon endroit
        // Trouver la position d'insertion
        int pos = comb->nb_tuiles;
        if (tuile.type == NORMAL) {
            for (int i = 0; i < comb->nb_tuiles; i++) {
                if (comb->tuiles[i].type == NORMAL && comb->tuiles[i].valeur > tuile.valeur) {
                    pos = i;
                    break;
                }
            }
        }
        
        // Décaler les tuiles
        for (int i = comb->nb_tuiles; i > pos; i--) {
            comb_test.tuiles[i] = comb_test.tuiles[i - 1];
        }
        comb_test.tuiles[pos] = tuile;
    } else { // Série - ajouter à la fin
        // Vérifier que la couleur n'est pas déjà utilisée
        if (tuile.type == NORMAL) {
            for (int i = 0; i < comb->nb_tuiles; i++) {
                if (comb->tuiles[i].type == NORMAL && 
                    comb->tuiles[i].couleur == tuile.couleur) {
                    return 0; // Couleur déjà utilisée
                }
            }
        }
    }
    
    if (!est_combinaison_valide(&comb_test)) {
        return 0;
    }
    
    // Appliquer les modifications
    *comb = comb_test;
    return 1;
}

// Retirer une tuile d'une combinaison
int retirer_tuile_combinaison(Combinaison *comb, int index) {
    if (index < 0 || index >= comb->nb_tuiles) {
        return 0;
    }
    
    // Ne peut retirer que si la combinaison a plus de 3 tuiles
    if (comb->nb_tuiles <= 3) {
        return 0;
    }
    
    // Ne peut retirer d'une combinaison avec joker
    if (a_joker(comb)) {
        return 0;
    }
    
    // Créer une nouvelle combinaison sans cette tuile
    Combinaison comb_test;
    comb_test.type = comb->type;
    comb_test.nb_tuiles = comb->nb_tuiles - 1;
    
    int j = 0;
    for (int i = 0; i < comb->nb_tuiles; i++) {
        if (i != index) {
            comb_test.tuiles[j++] = comb->tuiles[i];
        }
    }
    
    if (!est_combinaison_valide(&comb_test)) {
        return 0;
    }
    
    *comb = comb_test;
    return 1;
}

// Remplacer une tuile dans une combinaison
int remplacer_tuile_combinaison(Combinaison *comb, int index, Tuile nouvelle_tuile) {
    if (index < 0 || index >= comb->nb_tuiles) {
        return 0;
    }
    
    Tuile ancienne_tuile = comb->tuiles[index];
    
    // Si on remplace un joker, c'est une récupération de joker
    if (ancienne_tuile.type == JOKER) {
        // La nouvelle tuile doit correspondre exactement à ce que le joker remplace
        comb->tuiles[index] = nouvelle_tuile;
        
        if (!est_combinaison_valide(comb)) {
            // Restaurer
            comb->tuiles[index] = ancienne_tuile;
            return 0;
        }
        return 1;
    }
    
    // Remplacer une tuile normale
    comb->tuiles[index] = nouvelle_tuile;
    
    if (!est_combinaison_valide(comb)) {
        // Restaurer
        comb->tuiles[index] = ancienne_tuile;
        return 0;
    }
    
    return 1;
}

// Vérifier que toutes les combinaisons du plateau sont valides
int plateau_est_valide(Plateau *plateau) {
    for (int i = 0; i < plateau->nb_combinaisons; i++) {
        if (!est_combinaison_valide(&plateau->combinaisons[i])) {
            return 0;
        }
    }
    return 1;
}

// Calculer le total des points de plusieurs combinaisons
int calculer_total_points_combinaisons(Combinaison *combs, int nb_combs) {
    int total = 0;
    for (int i = 0; i < nb_combs; i++) {
        total += calculer_points_combinaison_complete(&combs[i]);
    }
    return total;
}

// Vérifier si un joueur peut faire 30 points avec ses tuiles
int peut_faire_30_points(Joueur *joueur) {
    // Algorithme simplifié : vérifier toutes les combinaisons possibles de 3 à 14 tuiles
    // Version optimisée : chercher les meilleures combinaisons
    
    // Trier les tuiles par valeur et couleur pour faciliter la recherche
    Tuile tuiles_triees[14];
    memcpy(tuiles_triees, joueur->tuiles, joueur->nb_tuiles * sizeof(Tuile));
    
    // Trier par couleur puis par valeur
    for (int i = 0; i < joueur->nb_tuiles - 1; i++) {
        for (int j = 0; j < joueur->nb_tuiles - i - 1; j++) {
            if (tuiles_triees[j].couleur > tuiles_triees[j + 1].couleur ||
                (tuiles_triees[j].couleur == tuiles_triees[j + 1].couleur &&
                 tuiles_triees[j].valeur > tuiles_triees[j + 1].valeur)) {
                Tuile temp = tuiles_triees[j];
                tuiles_triees[j] = tuiles_triees[j + 1];
                tuiles_triees[j + 1] = temp;
            }
        }
    }
    
    // Chercher des suites possibles
    for (int debut = 0; debut < joueur->nb_tuiles - 2; debut++) {
        for (int fin = debut + 2; fin < joueur->nb_tuiles; fin++) {
            Combinaison comb;
            comb.type = 0; // Suite
            comb.nb_tuiles = fin - debut + 1;
            for (int i = 0; i < comb.nb_tuiles; i++) {
                comb.tuiles[i] = tuiles_triees[debut + i];
            }
            
            if (est_combinaison_valide(&comb)) {
                int points = calculer_points_combinaison_complete(&comb);
                if (points >= 30) {
                    return 1;
                }
            }
        }
    }
    
    // Chercher des séries possibles
    for (int i = 0; i < joueur->nb_tuiles - 2; i++) {
        for (int j = i + 1; j < joueur->nb_tuiles - 1; j++) {
            for (int k = j + 1; k < joueur->nb_tuiles; k++) {
                Combinaison comb;
                comb.type = 1; // Série
                comb.nb_tuiles = 3;
                comb.tuiles[0] = tuiles_triees[i];
                comb.tuiles[1] = tuiles_triees[j];
                comb.tuiles[2] = tuiles_triees[k];
                
                if (est_combinaison_valide(&comb)) {
                    int points = calculer_points_combinaison_complete(&comb);
                    if (points >= 30) {
                        return 1;
                    }
                }
            }
        }
    }
    
    return 0;
}

// Vérifier si on peut récupérer un joker
int peut_recuperer_joker(Plateau *plateau, int index_comb, int index_tuile, Tuile tuile_remplacement) {
    if (index_comb < 0 || index_comb >= plateau->nb_combinaisons) {
        return 0;
    }
    
    Combinaison *comb = &plateau->combinaisons[index_comb];
    
    if (index_tuile < 0 || index_tuile >= comb->nb_tuiles) {
        return 0;
    }
    
    if (comb->tuiles[index_tuile].type != JOKER) {
        return 0;
    }
    
    // Tester le remplacement
    Tuile joker = comb->tuiles[index_tuile];
    comb->tuiles[index_tuile] = tuile_remplacement;
    
    int valide = est_combinaison_valide(comb);
    
    // Restaurer
    comb->tuiles[index_tuile] = joker;
    
    return valide;
}

// Récupérer un joker du plateau
int recuperer_joker(Plateau *plateau, int index_comb, int index_tuile, Tuile tuile_remplacement, Tuile *joker_recupere) {
    if (!peut_recuperer_joker(plateau, index_comb, index_tuile, tuile_remplacement)) {
        return 0;
    }
    
    Combinaison *comb = &plateau->combinaisons[index_comb];
    *joker_recupere = comb->tuiles[index_tuile];
    comb->tuiles[index_tuile] = tuile_remplacement;
    
    return 1;
}
