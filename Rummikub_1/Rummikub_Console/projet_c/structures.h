#ifndef STRUCTURES_H
#define STRUCTURES_H

// Enumération pour les couleurs des tuiles
typedef enum {
    JAUNE = 0,
    ROUGE = 1,
    NOIR = 2,
    BLEU = 3
} Couleur;

// Enumération pour le type de tuile
typedef enum {
    NORMAL = 0,
    JOKER = 1
} TypeTuile;

// Structure représentant une tuile
typedef struct {
    int valeur;        // 1-13 pour tuiles normales, 0 pour joker
    Couleur couleur;   // Ignoré si joker
    TypeTuile type;    // NORMAL ou JOKER
} Tuile;

// Structure représentant un joueur
typedef struct {
    Tuile tuiles[14];  // Chevalet du joueur (maximum 14 tuiles)
    int nb_tuiles;     // Nombre de tuiles actuellement sur le chevalet
    char pseudo[50];   // Pseudo du joueur
    int score;         // Score du joueur
    int est_ia;        // 1 si joueur IA, 0 si humain
} Joueur;

// Structure représentant la pioche
typedef struct {
    Tuile tuiles[106]; // 104 tuiles + 2 jokers
    int nb_tuiles;     // Nombre de tuiles restantes dans la pioche
} Pioche;

// Structure représentant une combinaison sur le plateau
typedef struct {
    Tuile tuiles[13];  // Maximum 13 tuiles dans une combinaison
    int nb_tuiles;     // Nombre de tuiles dans la combinaison
    int type;          // 0 = suite, 1 = série
} Combinaison;

// Structure représentant le plateau de jeu
typedef struct {
    Combinaison combinaisons[50]; // Combinaisons sur le plateau
    int nb_combinaisons;          // Nombre de combinaisons sur le plateau
} Plateau;

// Fonctions utilitaires pour les structures
Tuile creer_tuile(int valeur, Couleur couleur);
Tuile creer_joker(void);
void initialiser_joueur(Joueur *joueur, const char *pseudo, int est_ia);
void initialiser_pioche(Pioche *pioche);
void initialiser_plateau(Plateau *plateau);

#endif // STRUCTURES_H

