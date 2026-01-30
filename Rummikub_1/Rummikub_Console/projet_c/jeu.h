#ifndef JEU_H
#define JEU_H

#include "structures.h"

// Initialisation du jeu
void creer_toutes_tuiles(Pioche *pioche);
void melanger_pioche(Pioche *pioche);
void distribuer_tuiles(Pioche *pioche, Joueur *joueurs, int nb_joueurs);
int choisir_premier_joueur(Joueur *joueurs, int nb_joueurs, Pioche *pioche);

// Validation des combinaisons
int est_suite_valide(Tuile *tuiles, int nb_tuiles);
int est_serie_valide(Tuile *tuiles, int nb_tuiles);
int calculer_points_combinaison(Tuile *tuiles, int nb_tuiles);
int verifier_30_points_initiaux(Tuile *tuiles, int nb_tuiles);

// Gestion de la pioche
Tuile piocher_tuile(Pioche *pioche);
void ajouter_tuile_joueur(Joueur *joueur, Tuile tuile);
void retirer_tuile_joueur(Joueur *joueur, int index);

// Utilitaires
const char* couleur_to_string(Couleur couleur);
void afficher_tuile_console(Tuile t);

#endif // JEU_H

