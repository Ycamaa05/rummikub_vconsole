#include "structures.h"
#include <string.h>

// Fonction pour créer une tuile normale
Tuile creer_tuile(int valeur, Couleur couleur) {
    Tuile t;
    t.valeur = valeur;
    t.couleur = couleur;
    t.type = NORMAL;
    return t;
}

// Fonction pour créer un joker
Tuile creer_joker(void) {
    Tuile t;
    t.valeur = 0;
    t.couleur = JAUNE; // Couleur arbitraire pour joker
    t.type = JOKER;
    return t;
}

// Fonction pour initialiser un joueur
void initialiser_joueur(Joueur *joueur, const char *pseudo, int est_ia) {
    strncpy(joueur->pseudo, pseudo, 49);
    joueur->pseudo[49] = '\0';
    joueur->nb_tuiles = 0;
    joueur->score = 0;
    joueur->est_ia = est_ia;
}

// Fonction pour initialiser la pioche
void initialiser_pioche(Pioche *pioche) {
    pioche->nb_tuiles = 0;
}

// Fonction pour initialiser le plateau
void initialiser_plateau(Plateau *plateau) {
    plateau->nb_combinaisons = 0;
}

