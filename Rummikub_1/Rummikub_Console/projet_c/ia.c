#include "ia.h"
#include "jeu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Trouver toutes les combinaisons possibles avec les tuiles du joueur
int trouver_combinaisons_possibles_ia(Joueur *joueur, Combinaison *resultats, int max_resultats) {
    int nb_resultats = 0;
    
    // Trier les tuiles
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
    
    // Chercher des suites possibles (même couleur, valeurs consécutives)
    for (int debut = 0; debut < joueur->nb_tuiles - 2 && nb_resultats < max_resultats; debut++) {
        for (int fin = debut + 2; fin < joueur->nb_tuiles && nb_resultats < max_resultats; fin++) {
            Combinaison comb;
            comb.type = 0; // Suite
            comb.nb_tuiles = fin - debut + 1;
            for (int i = 0; i < comb.nb_tuiles; i++) {
                comb.tuiles[i] = tuiles_triees[debut + i];
            }
            
            if (est_combinaison_valide(&comb)) {
                resultats[nb_resultats++] = comb;
            }
        }
    }
    
    // Chercher des séries possibles (même valeur, couleurs différentes)
    for (int i = 0; i < joueur->nb_tuiles - 2 && nb_resultats < max_resultats; i++) {
        for (int j = i + 1; j < joueur->nb_tuiles - 1 && nb_resultats < max_resultats; j++) {
            for (int k = j + 1; k < joueur->nb_tuiles && nb_resultats < max_resultats; k++) {
                // Vérifier que les valeurs sont identiques (ou jokers)
                int val_i = tuiles_triees[i].type == JOKER ? -1 : tuiles_triees[i].valeur;
                int val_j = tuiles_triees[j].type == JOKER ? -1 : tuiles_triees[j].valeur;
                int val_k = tuiles_triees[k].type == JOKER ? -1 : tuiles_triees[k].valeur;
                
                if (val_i == -1 || val_j == -1 || val_k == -1) {
                    // Au moins un joker, vérifier les autres
                    int val_ref = (val_i != -1) ? val_i : ((val_j != -1) ? val_j : val_k);
                    if (val_i != -1 && val_i != val_ref) continue;
                    if (val_j != -1 && val_j != val_ref) continue;
                    if (val_k != -1 && val_k != val_ref) continue;
                } else {
                    if (val_i != val_j || val_j != val_k) continue;
                }
                
                // Vérifier que les couleurs sont différentes (si pas de joker)
                if (tuiles_triees[i].type == NORMAL && tuiles_triees[j].type == NORMAL &&
                    tuiles_triees[i].couleur == tuiles_triees[j].couleur) continue;
                if (tuiles_triees[i].type == NORMAL && tuiles_triees[k].type == NORMAL &&
                    tuiles_triees[i].couleur == tuiles_triees[k].couleur) continue;
                if (tuiles_triees[j].type == NORMAL && tuiles_triees[k].type == NORMAL &&
                    tuiles_triees[j].couleur == tuiles_triees[k].couleur) continue;
                
                Combinaison comb;
                comb.type = 1; // Série
                comb.nb_tuiles = 3;
                comb.tuiles[0] = tuiles_triees[i];
                comb.tuiles[1] = tuiles_triees[j];
                comb.tuiles[2] = tuiles_triees[k];
                
                if (est_combinaison_valide(&comb)) {
                    resultats[nb_resultats++] = comb;
                }
            }
        }
    }
    
    return nb_resultats;
}

// Vérifier si l'IA peut faire son premier coup (30 points)
int ia_peut_commencer(Joueur *joueur, CoupIA *coup) {
    Combinaison combinaisons_possibles[20];
    int nb_combs = trouver_combinaisons_possibles_ia(joueur, combinaisons_possibles, 20);
    
    // Chercher une combinaison de 30 points ou plus
    for (int i = 0; i < nb_combs; i++) {
        int points = calculer_points_combinaison_complete(&combinaisons_possibles[i]);
        if (points >= 30) {
            coup->type_action = 0; // Poser combinaison
            coup->nouvelle_combinaison = combinaisons_possibles[i];
            coup->points_gagnes = points;
            return 1;
        }
    }
    
    // Chercher plusieurs combinaisons qui totalisent 30 points
    for (int i = 0; i < nb_combs; i++) {
        for (int j = i + 1; j < nb_combs; j++) {
            int total = calculer_points_combinaison_complete(&combinaisons_possibles[i]) +
                       calculer_points_combinaison_complete(&combinaisons_possibles[j]);
            if (total >= 30) {
                // Prendre la première combinaison (on pourra améliorer pour prendre la meilleure)
                coup->type_action = 0;
                coup->nouvelle_combinaison = combinaisons_possibles[i];
                coup->points_gagnes = total;
                return 1;
            }
        }
    }
    
    return 0;
}

// Évaluer un coup possible
int evaluer_coup(CoupIA *coup, Joueur *joueur, Plateau *plateau) {
    int score = 0;
    
    switch (coup->type_action) {
        case 0: // Poser combinaison
            score = calculer_points_combinaison_complete(&coup->nouvelle_combinaison) * 10;
            score -= coup->nouvelle_combinaison.nb_tuiles; // Pénalité pour utiliser beaucoup de tuiles
            break;
            
        case 1: // Ajouter tuile à combinaison existante
            score = 50; // Bon coup car on utilise le plateau existant
            break;
            
        case 2: // Diviser combinaison
            score = 30;
            break;
            
        case 3: // Remplacer tuile
            score = 20;
            break;
            
        case 4: // Récupérer joker
            score = 100; // Très bon coup
            break;
    }
    
    return score;
}

// Fonction principale de l'IA : choisir le meilleur coup
int choisir_coup_ia(Joueur *joueur, Plateau *plateau, CoupIA *meilleur_coup) {
    CoupIA coups_possibles[50];
    int nb_coups = 0;
    
    // Si le joueur n'a pas encore joué, vérifier s'il peut commencer
    // (on suppose qu'on passe un flag pour savoir si c'est le premier coup)
    // Pour simplifier, on cherche toujours le meilleur coup possible
    
    // 1. Chercher à poser de nouvelles combinaisons
    Combinaison combinaisons_possibles[20];
    int nb_combs = trouver_combinaisons_possibles_ia(joueur, combinaisons_possibles, 20);
    
    for (int i = 0; i < nb_combs && nb_coups < 50; i++) {
        coups_possibles[nb_coups].type_action = 0;
        coups_possibles[nb_coups].nouvelle_combinaison = combinaisons_possibles[i];
        coups_possibles[nb_coups].points_gagnes = calculer_points_combinaison_complete(&combinaisons_possibles[i]);
        coups_possibles[nb_coups].index_comb_plateau = -1;
        nb_coups++;
    }
    
    // 2. Chercher à ajouter des tuiles aux combinaisons existantes
    for (int i = 0; i < plateau->nb_combinaisons && nb_coups < 50; i++) {
        for (int j = 0; j < joueur->nb_tuiles && nb_coups < 50; j++) {
            Combinaison comb_test = plateau->combinaisons[i];
            if (ajouter_tuile_combinaison(&comb_test, joueur->tuiles[j])) {
                coups_possibles[nb_coups].type_action = 1;
                coups_possibles[nb_coups].index_comb_plateau = i;
                coups_possibles[nb_coups].index_tuile_chevalet = j;
                coups_possibles[nb_coups].nouvelle_combinaison = comb_test;
                coups_possibles[nb_coups].points_gagnes = calculer_points_combinaison_complete(&comb_test);
                nb_coups++;
            }
        }
    }
    
    // 3. Chercher à récupérer des jokers
    for (int i = 0; i < plateau->nb_combinaisons && nb_coups < 50; i++) {
        for (int j = 0; j < plateau->combinaisons[i].nb_tuiles && nb_coups < 50; j++) {
            if (plateau->combinaisons[i].tuiles[j].type == JOKER) {
                // Chercher une tuile dans le chevalet qui peut remplacer le joker
                for (int k = 0; k < joueur->nb_tuiles && nb_coups < 50; k++) {
                    if (peut_recuperer_joker(plateau, i, j, joueur->tuiles[k])) {
                        coups_possibles[nb_coups].type_action = 4;
                        coups_possibles[nb_coups].index_comb_plateau = i;
                        coups_possibles[nb_coups].index_tuile_combinaison = j;
                        coups_possibles[nb_coups].index_tuile_chevalet = k;
                        coups_possibles[nb_coups].points_gagnes = 100; // Très bon coup
                        nb_coups++;
                    }
                }
            }
        }
    }
    
    // Si aucun coup trouvé, retourner 0 (l'IA devra piocher)
    if (nb_coups == 0) {
        return 0;
    }
    
    // Choisir le meilleur coup
    int meilleur_score = -1;
    int index_meilleur = -1;
    
    for (int i = 0; i < nb_coups; i++) {
        int score = evaluer_coup(&coups_possibles[i], joueur, plateau);
        if (score > meilleur_score) {
            meilleur_score = score;
            index_meilleur = i;
        }
    }
    
    if (index_meilleur >= 0) {
        *meilleur_coup = coups_possibles[index_meilleur];
        return 1;
    }
    
    return 0;
}
