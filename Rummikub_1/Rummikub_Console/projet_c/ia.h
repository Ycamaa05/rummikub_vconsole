#ifndef IA_H
#define IA_H

#include "structures.h"
#include "plateau.h"

// Structure pour stocker un coup possible de l'IA
typedef struct {
    int type_action; // 0=poser combinaison, 1=ajouter tuile, 2=diviser, 3=remplacer, 4=récupérer joker
    int index_comb_plateau; // Index de la combinaison sur le plateau (si applicable)
    Combinaison nouvelle_combinaison; // Nouvelle combinaison à poser
    int index_tuile_chevalet; // Index de la tuile dans le chevalet
    int index_tuile_combinaison; // Index de la tuile dans la combinaison (si applicable)
    int points_gagnes; // Points gagnés avec ce coup
} CoupIA;

// Fonction principale de l'IA : choisir le meilleur coup
int choisir_coup_ia(Joueur *joueur, Plateau *plateau, CoupIA *meilleur_coup);

// Vérifier si l'IA peut faire son premier coup (30 points)
int ia_peut_commencer(Joueur *joueur, CoupIA *coup);

// Trouver toutes les combinaisons possibles avec les tuiles du joueur
int trouver_combinaisons_possibles_ia(Joueur *joueur, Combinaison *resultats, int max_resultats);

// Évaluer un coup possible
int evaluer_coup(CoupIA *coup, Joueur *joueur, Plateau *plateau);

#endif // IA_H
