#include "jeu.h"
#include "structures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Création de toutes les tuiles (104 tuiles + 2 jokers)
void creer_toutes_tuiles(Pioche *pioche) {
    int index = 0;
    
    // Créer 2 exemplaires de chaque tuile (1-13) pour chaque couleur
    for (int exemplaire = 0; exemplaire < 2; exemplaire++) {
        for (int valeur = 1; valeur <= 13; valeur++) {
            for (int couleur = 0; couleur < 4; couleur++) {
                pioche->tuiles[index] = creer_tuile(valeur, (Couleur)couleur);
                index++;
            }
        }
    }
    
    // Ajouter les 2 jokers
    pioche->tuiles[index++] = creer_joker();
    pioche->tuiles[index++] = creer_joker();
    
    pioche->nb_tuiles = 106;
}

// Mélange de la pioche (algorithme de Fisher-Yates)
void melanger_pioche(Pioche *pioche) {
    srand((unsigned int)time(NULL));
    
    for (int i = pioche->nb_tuiles - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Tuile temp = pioche->tuiles[i];
        pioche->tuiles[i] = pioche->tuiles[j];
        pioche->tuiles[j] = temp;
    }
}

// Distribution de 14 tuiles à chaque joueur
void distribuer_tuiles(Pioche *pioche, Joueur *joueurs, int nb_joueurs) {
    for (int tour = 0; tour < 14; tour++) {
        for (int j = 0; j < nb_joueurs; j++) {
            if (pioche->nb_tuiles > 0) {
                Tuile tuile = piocher_tuile(pioche);
                ajouter_tuile_joueur(&joueurs[j], tuile);
            }
        }
    }
}

// Choix du premier joueur (celui qui pioche la tuile de plus grande valeur)
int choisir_premier_joueur(Joueur *joueurs, int nb_joueurs, Pioche *pioche) {
    int valeurs_piochees[4] = {0};
    int max_valeur = -1;
    int joueur_commence = 0;
    
    // Chaque joueur pioche une tuile
    for (int i = 0; i < nb_joueurs; i++) {
        if (pioche->nb_tuiles > 0) {
            Tuile tuile = piocher_tuile(pioche);
            valeurs_piochees[i] = tuile.valeur;
            
            // Remettre la tuile dans la pioche (on ne la garde pas)
            if (pioche->nb_tuiles < 106) {
                pioche->tuiles[pioche->nb_tuiles] = tuile;
                pioche->nb_tuiles++;
            }
            
            if (valeurs_piochees[i] > max_valeur) {
                max_valeur = valeurs_piochees[i];
                joueur_commence = i;
            }
        }
    }
    
    // Remélanger la pioche après avoir choisi le premier joueur
    melanger_pioche(pioche);
    
    return joueur_commence;
}

// Piocher une tuile de la pioche
Tuile piocher_tuile(Pioche *pioche) {
    if (pioche->nb_tuiles > 0) {
        Tuile tuile = pioche->tuiles[pioche->nb_tuiles - 1];
        pioche->nb_tuiles--;
        return tuile;
    }
    // Retourner une tuile invalide si la pioche est vide
    Tuile tuile_vide = {0, JAUNE, NORMAL};
    return tuile_vide;
}

// Ajouter une tuile au chevalet d'un joueur
void ajouter_tuile_joueur(Joueur *joueur, Tuile tuile) {
    if (joueur->nb_tuiles < 14) {
        joueur->tuiles[joueur->nb_tuiles] = tuile;
        joueur->nb_tuiles++;
    }
}

// Retirer une tuile du chevalet d'un joueur
void retirer_tuile_joueur(Joueur *joueur, int index) {
    if (index >= 0 && index < joueur->nb_tuiles) {
        // Décaler les tuiles vers la gauche
        for (int i = index; i < joueur->nb_tuiles - 1; i++) {
            joueur->tuiles[i] = joueur->tuiles[i + 1];
        }
        joueur->nb_tuiles--;
    }
}

// Vérifier si les tuiles forment une suite valide (même couleur, valeurs consécutives)
int est_suite_valide(Tuile *tuiles, int nb_tuiles) {
    if (nb_tuiles < 3) return 0;
    
    // Trier les tuiles par valeur (tri simple à bulles)
    Tuile tuiles_triees[13];
    memcpy(tuiles_triees, tuiles, nb_tuiles * sizeof(Tuile));
    
    for (int i = 0; i < nb_tuiles - 1; i++) {
        for (int j = 0; j < nb_tuiles - i - 1; j++) {
            if (tuiles_triees[j].valeur > tuiles_triees[j + 1].valeur) {
                Tuile temp = tuiles_triees[j];
                tuiles_triees[j] = tuiles_triees[j + 1];
                tuiles_triees[j + 1] = temp;
            }
        }
    }
    
    // Trouver la couleur de référence (ignorer les jokers)
    Couleur couleur_ref = JAUNE;
    int trouve_couleur = 0;
    for (int i = 0; i < nb_tuiles; i++) {
        if (tuiles_triees[i].type == NORMAL) {
            couleur_ref = tuiles_triees[i].couleur;
            trouve_couleur = 1;
            break;
        }
    }
    
    if (!trouve_couleur) return 0; // Pas de tuile normale pour référence
    
    // Vérifier que toutes les tuiles normales ont la même couleur
    for (int i = 0; i < nb_tuiles; i++) {
        if (tuiles_triees[i].type == NORMAL && tuiles_triees[i].couleur != couleur_ref) {
            return 0;
        }
    }
    
    // Vérifier que les valeurs sont consécutives
    int valeur_attendue = tuiles_triees[0].valeur;
    int jokers_utilises = 0;
    
    for (int i = 0; i < nb_tuiles; i++) {
        if (tuiles_triees[i].type == JOKER) {
            jokers_utilises++;
            valeur_attendue++; // Le joker remplace cette valeur
        } else {
            if (tuiles_triees[i].valeur != valeur_attendue) {
                return 0;
            }
            valeur_attendue++;
        }
    }
    
    return 1;
}

// Vérifier si les tuiles forment une série valide (même valeur, couleurs différentes)
int est_serie_valide(Tuile *tuiles, int nb_tuiles) {
    if (nb_tuiles < 3) return 0; // Minimum 3 tuiles, maximum 4 (une par couleur)
    
    // Trouver la valeur de référence (ignorer les jokers)
    int valeur_ref = -1;
    int trouve_valeur = 0;
    for (int i = 0; i < nb_tuiles; i++) {
        if (tuiles[i].type == NORMAL) {
            valeur_ref = tuiles[i].valeur;
            trouve_valeur = 1;
            break;
        }
    }
    
    if (!trouve_valeur) return 0; // Pas de tuile normale pour référence
    
    // Vérifier que toutes les tuiles normales ont la même valeur
    int couleurs_utilisees[4] = {0};
    int jokers_count = 0;
    
    for (int i = 0; i < nb_tuiles; i++) {
        if (tuiles[i].type == JOKER) {
            jokers_count++;
        } else {
            if (tuiles[i].valeur != valeur_ref) {
                return 0;
            }
            // Vérifier qu'on n'a pas deux tuiles de la même couleur
            if (couleurs_utilisees[tuiles[i].couleur]) {
                return 0;
            }
            couleurs_utilisees[tuiles[i].couleur] = 1;
        }
    }
    
    // Vérifier qu'on n'a pas trop de jokers
    if (jokers_count > 1) return 0;
    
    return 1;
}

// Calculer les points d'une combinaison (version améliorée pour les jokers)
int calculer_points_combinaison(Tuile *tuiles, int nb_tuiles) {
    int points = 0;
    for (int i = 0; i < nb_tuiles; i++) {
        if (tuiles[i].type == JOKER) {
            // Pour un joker dans une suite, déterminer sa valeur selon sa position
            // Chercher les valeurs min et max des tuiles normales
            int min_val = 14, max_val = 0;
            int nb_normales = 0;
            for (int j = 0; j < nb_tuiles; j++) {
                if (tuiles[j].type == NORMAL) {
                    if (tuiles[j].valeur < min_val) min_val = tuiles[j].valeur;
                    if (tuiles[j].valeur > max_val) max_val = tuiles[j].valeur;
                    nb_normales++;
                }
            }
            
            if (nb_normales > 0) {
                // Estimer la valeur du joker (approximation basée sur la moyenne)
                int val_estimee = (min_val + max_val) / 2;
                if (val_estimee < 1) val_estimee = 1;
                if (val_estimee > 13) val_estimee = 13;
                points += val_estimee;
            } else {
                // Seulement des jokers, valeur par défaut
                points += 10;
            }
        } else {
            points += tuiles[i].valeur;
        }
    }
    return points;
}

// Vérifier si le joueur peut commencer (au moins 30 points)
int verifier_30_points_initiaux(Tuile *tuiles, int nb_tuiles) {
    // Version simplifiée : vérifier toutes les combinaisons possibles de 3 tuiles ou plus
    // Cette fonction est partielle et peut être améliorée
    
    // Vérifier les suites possibles
    for (int i = 0; i < nb_tuiles - 2; i++) {
        for (int j = i + 1; j < nb_tuiles - 1; j++) {
            for (int k = j + 1; k < nb_tuiles; k++) {
                Tuile combinaison[3] = {tuiles[i], tuiles[j], tuiles[k]};
                if (est_suite_valide(combinaison, 3) || est_serie_valide(combinaison, 3)) {
                    int points = calculer_points_combinaison(combinaison, 3);
                    if (points >= 30) {
                        return 1;
                    }
                }
            }
        }
    }
    
    // Vérifier les combinaisons de 4 tuiles
    if (nb_tuiles >= 4) {
        for (int i = 0; i < nb_tuiles - 3; i++) {
            for (int j = i + 1; j < nb_tuiles - 2; j++) {
                for (int k = j + 1; k < nb_tuiles - 1; k++) {
                    for (int l = k + 1; l < nb_tuiles; l++) {
                        Tuile combinaison[4] = {tuiles[i], tuiles[j], tuiles[k], tuiles[l]};
                        if (est_suite_valide(combinaison, 4) || est_serie_valide(combinaison, 4)) {
                            int points = calculer_points_combinaison(combinaison, 4);
                            if (points >= 30) {
                                return 1;
                            }
                        }
                    }
                }
            }
        }
    }
    
    return 0;
}

// Convertir une couleur en chaîne de caractères
const char* couleur_to_string(Couleur couleur) {
    switch (couleur) {
        case JAUNE: return "Jaune";
        case ROUGE: return "Rouge";
        case NOIR: return "Noir";
        case BLEU: return "Bleu";
        default: return "Inconnu";
    }
}

// Note: afficher_tuile_console est définie dans main_console.c

