#ifndef PLATEAU_H
#define PLATEAU_H

#include "structures.h"

// Validation des combinaisons
int est_combinaison_valide(Combinaison *comb);
int calculer_points_combinaison_complete(Combinaison *comb);
int a_joker(Combinaison *comb);

// Gestion du plateau
int ajouter_combinaison_plateau(Plateau *plateau, Combinaison *comb);
int retirer_combinaison_plateau(Plateau *plateau, int index);
int modifier_combinaison_plateau(Plateau *plateau, int index, Combinaison *nouvelle_comb);
int diviser_combinaison(Plateau *plateau, int index_comb, int index_division, Combinaison *comb1, Combinaison *comb2);

// Actions sur les combinaisons
int ajouter_tuile_combinaison(Combinaison *comb, Tuile tuile);
int retirer_tuile_combinaison(Combinaison *comb, int index);
int remplacer_tuile_combinaison(Combinaison *comb, int index, Tuile nouvelle_tuile);

// Vérification de l'état du plateau
int plateau_est_valide(Plateau *plateau);
int calculer_total_points_combinaisons(Combinaison *combs, int nb_combs);

// Recherche de combinaisons possibles
int trouver_combinaisons_possibles(Joueur *joueur, Combinaison *resultats, int max_resultats);
int peut_faire_30_points(Joueur *joueur);

// Gestion des jokers
int peut_recuperer_joker(Plateau *plateau, int index_comb, int index_tuile, Tuile tuile_remplacement);
int recuperer_joker(Plateau *plateau, int index_comb, int index_tuile, Tuile tuile_remplacement, Tuile *joker_recupere);

#endif // PLATEAU_H
