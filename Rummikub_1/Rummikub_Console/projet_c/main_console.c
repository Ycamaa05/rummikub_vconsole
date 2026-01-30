#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "structures.h"
#include "jeu.h"
#include "fichiers.h"
#include "plateau.h"
#include "ia.h"

#define NB_JOUEURS_MAX 4
#define TIMER_TOUR 60 // 60 secondes par tour (variante optionnelle)

// Structure pour suivre l'état du premier coup de chaque joueur
typedef struct {
    int a_joue_premier_coup[NB_JOUEURS_MAX];
} EtatPremierCoup;

// Fonction pour afficher une tuile en console
void afficher_tuile_console(Tuile t) {
    if (t.type == JOKER) {
        printf("Joker");
    } else {
        const char* couleur_str = couleur_to_string(t.couleur);
        char couleur_lettre = couleur_str[0]; // Première lettre
        printf("%d%c", t.valeur, couleur_lettre);
    }
}

// Fonction pour afficher le chevalet d'un joueur
void afficher_chevalet_console(Joueur *joueur) {
    printf("\n=== Chevalet de %s (%d tuiles) ===\n", joueur->pseudo, joueur->nb_tuiles);
    for (int i = 0; i < joueur->nb_tuiles; i++) {
        printf("[%d] ", i);
        afficher_tuile_console(joueur->tuiles[i]);
        printf("  ");
        if ((i + 1) % 7 == 0) printf("\n"); // Nouvelle ligne tous les 7 tuiles
    }
    printf("\n");
}

// Fonction pour afficher le plateau
void afficher_plateau_console(Plateau *plateau) {
    printf("\n=== PLATEAU ===\n");
    if (plateau->nb_combinaisons == 0) {
        printf("Aucune combinaison sur le plateau.\n");
    } else {
        for (int c = 0; c < plateau->nb_combinaisons; c++) {
            printf("Combinaison %d (%s): ", c + 1, 
                   plateau->combinaisons[c].type == 0 ? "Suite" : "Série");
            for (int i = 0; i < plateau->combinaisons[c].nb_tuiles; i++) {
                afficher_tuile_console(plateau->combinaisons[c].tuiles[i]);
                printf(" ");
            }
            int points = calculer_points_combinaison_complete(&plateau->combinaisons[c]);
            printf(" [%d points]", points);
            printf("\n");
        }
    }
    printf("\n");
}

// Fonction pour demander les pseudos des joueurs
void demander_pseudos(Joueur *joueurs, int nb_joueurs) {
    printf("\n=== Configuration des joueurs ===\n");
    for (int i = 0; i < nb_joueurs; i++) {
        char pseudo[50];
        int est_ia = 0;
        
        printf("Joueur %d:\n", i + 1);
        printf("  - Pseudo: ");
        fgets(pseudo, sizeof(pseudo), stdin);
        pseudo[strcspn(pseudo, "\n")] = 0;
        
        if (strlen(pseudo) == 0) {
            sprintf(pseudo, "Joueur%d", i + 1);
        }
        
        printf("  - Est-ce un joueur IA ? (0=Non, 1=Oui): ");
        scanf("%d", &est_ia);
        getchar();
        
        initialiser_joueur(&joueurs[i], pseudo, est_ia);
        if (!est_ia) {
            sauvegarder_pseudo(pseudo);
        }
        printf("✓ Joueur %d: %s %s\n", i + 1, pseudo, est_ia ? "(IA)" : "");
    }
}

// Fonction pour afficher le menu d'actions complet
void afficher_menu_complet(int premier_coup) {
    printf("\n=== Actions disponibles ===\n");
    if (premier_coup) {
        printf("⚠️  PREMIER COUP: Vous devez poser au moins 30 points!\n");
    }
    printf("1. Poser une nouvelle combinaison\n");
    printf("2. Ajouter une tuile à une combinaison existante\n");
    printf("3. Retirer une tuile d'une combinaison (et la réutiliser)\n");
    printf("4. Diviser une suite en deux suites\n");
    printf("5. Remplacer une tuile dans une combinaison\n");
    printf("6. Récupérer un joker du plateau\n");
    printf("7. Piocher une tuile (et passer son tour)\n");
    printf("8. Voir mon chevalet\n");
    printf("9. Voir le plateau\n");
    printf("10. Voir les informations de la pioche\n");
    printf("0. Quitter\n");
    printf("Votre choix: ");
}

// Fonction pour créer une combinaison depuis le chevalet
int creer_combinaison_depuis_chevalet(Joueur *joueur, Combinaison *comb) {
    printf("\n=== Création d'une combinaison ===\n");
    printf("Type de combinaison (0=Suite, 1=Série): ");
    int type;
    scanf("%d", &type);
    getchar();
    
    if (type != 0 && type != 1) {
        printf("❌ Type invalide\n");
        return 0;
    }
    
    comb->type = type;
    comb->nb_tuiles = 0;
    
    printf("Combien de tuiles (minimum 3) ? ");
    int nb_tuiles;
    scanf("%d", &nb_tuiles);
    getchar();
    
    if (nb_tuiles < 3 || nb_tuiles > joueur->nb_tuiles) {
        printf("❌ Nombre de tuiles invalide\n");
        return 0;
    }
    
    printf("Entrez les indices des tuiles de votre chevalet (séparés par des espaces): ");
    int indices[14];
    for (int i = 0; i < nb_tuiles; i++) {
        scanf("%d", &indices[i]);
        if (indices[i] < 0 || indices[i] >= joueur->nb_tuiles) {
            printf("❌ Indice invalide: %d\n", indices[i]);
            return 0;
        }
    }
    getchar();
    
    // Vérifier qu'on n'utilise pas deux fois la même tuile
    for (int i = 0; i < nb_tuiles; i++) {
        for (int j = i + 1; j < nb_tuiles; j++) {
            if (indices[i] == indices[j]) {
                printf("❌ Vous ne pouvez pas utiliser la même tuile deux fois\n");
                return 0;
            }
        }
    }
    
    // Créer la combinaison
    for (int i = 0; i < nb_tuiles; i++) {
        comb->tuiles[i] = joueur->tuiles[indices[i]];
    }
    comb->nb_tuiles = nb_tuiles;
    
    // Trier si c'est une suite
    if (type == 0) {
        for (int i = 0; i < nb_tuiles - 1; i++) {
            for (int j = 0; j < nb_tuiles - i - 1; j++) {
                if (comb->tuiles[j].valeur > comb->tuiles[j + 1].valeur) {
                    Tuile temp = comb->tuiles[j];
                    comb->tuiles[j] = comb->tuiles[j + 1];
                    comb->tuiles[j + 1] = temp;
                }
            }
        }
    }
    
    if (!est_combinaison_valide(comb)) {
        printf("❌ Combinaison invalide!\n");
        return 0;
    }
    
    // Retirer les tuiles du chevalet
    // Trier les indices en ordre décroissant pour éviter les problèmes de décalage
    for (int i = 0; i < nb_tuiles - 1; i++) {
        for (int j = 0; j < nb_tuiles - i - 1; j++) {
            if (indices[j] < indices[j + 1]) {
                int temp = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = temp;
            }
        }
    }
    
    for (int i = 0; i < nb_tuiles; i++) {
        retirer_tuile_joueur(joueur, indices[i]);
    }
    
    return 1;
}

// Fonction pour gérer le tour d'un joueur humain
int tour_joueur_humain(Joueur *joueur, Plateau *plateau, Pioche *pioche, int premier_coup, EtatPremierCoup *etat, int index_joueur) {
    int a_pose_tuile = 0;
    Plateau plateau_backup = *plateau; // Sauvegarde pour annuler si invalide
    
    while (1) {
        afficher_chevalet_console(joueur);
        afficher_plateau_console(plateau);
        afficher_menu_complet(premier_coup);
        
        int choix;
        scanf("%d", &choix);
        getchar();
        
        switch (choix) {
            case 1: { // Poser nouvelle combinaison
                Combinaison comb;
                if (creer_combinaison_depuis_chevalet(joueur, &comb)) {
                    if (ajouter_combinaison_plateau(plateau, &comb)) {
                        int points = calculer_points_combinaison_complete(&comb);
                        printf("✓ Combinaison posée! Points: %d\n", points);
                        a_pose_tuile = 1;
                        
                        // Vérifier si c'est le premier coup
                        if (premier_coup) {
                            int total = calculer_total_points_combinaisons(plateau->combinaisons, plateau->nb_combinaisons);
                            if (total < 30) {
                                printf("❌ ERREUR: Premier coup doit faire au moins 30 points!\n");
                                printf("   Total actuel: %d points\n", total);
                                // Annuler
                                *plateau = plateau_backup;
                                // Remettre les tuiles dans le chevalet
                                for (int i = 0; i < comb.nb_tuiles; i++) {
                                    ajouter_tuile_joueur(joueur, comb.tuiles[i]);
                                }
                                a_pose_tuile = 0;
                            } else {
                                printf("✓ Premier coup validé! Total: %d points\n", total);
                                etat->a_joue_premier_coup[index_joueur] = 1;
                            }
                        }
                    } else {
                        printf("❌ Impossible d'ajouter la combinaison au plateau\n");
                        // Remettre les tuiles
                        for (int i = 0; i < comb.nb_tuiles; i++) {
                            ajouter_tuile_joueur(joueur, comb.tuiles[i]);
                        }
                    }
                }
                break;
            }
            
            case 2: { // Ajouter tuile à combinaison existante
                if (plateau->nb_combinaisons == 0) {
                    printf("❌ Aucune combinaison sur le plateau\n");
                    break;
                }
                printf("Index de la combinaison (0-%d): ", plateau->nb_combinaisons - 1);
                int idx_comb;
                scanf("%d", &idx_comb);
                getchar();
                
                printf("Index de la tuile dans votre chevalet: ");
                int idx_tuile;
                scanf("%d", &idx_tuile);
                getchar();
                
                if (idx_comb < 0 || idx_comb >= plateau->nb_combinaisons ||
                    idx_tuile < 0 || idx_tuile >= joueur->nb_tuiles) {
                    printf("❌ Indices invalides\n");
                    break;
                }
                
                Combinaison comb_backup = plateau->combinaisons[idx_comb];
                Tuile tuile = joueur->tuiles[idx_tuile];
                
                if (ajouter_tuile_combinaison(&plateau->combinaisons[idx_comb], tuile)) {
                    if (plateau_est_valide(plateau)) {
                        retirer_tuile_joueur(joueur, idx_tuile);
                        printf("✓ Tuile ajoutée!\n");
                        a_pose_tuile = 1;
                    } else {
                        printf("❌ Cette action rend le plateau invalide\n");
                        plateau->combinaisons[idx_comb] = comb_backup;
                    }
                } else {
                    printf("❌ Impossible d'ajouter cette tuile à la combinaison\n");
                }
                break;
            }
            
            case 3: { // Retirer tuile d'une combinaison
                if (plateau->nb_combinaisons == 0) {
                    printf("❌ Aucune combinaison sur le plateau\n");
                    break;
                }
                printf("Index de la combinaison (0-%d): ", plateau->nb_combinaisons - 1);
                int idx_comb;
                scanf("%d", &idx_comb);
                getchar();
                
                if (idx_comb < 0 || idx_comb >= plateau->nb_combinaisons) {
                    printf("❌ Index invalide\n");
                    break;
                }
                
                if (a_joker(&plateau->combinaisons[idx_comb])) {
                    printf("❌ Impossible de retirer une tuile d'une combinaison contenant un joker\n");
                    break;
                }
                
                printf("Index de la tuile à retirer (0-%d): ", 
                       plateau->combinaisons[idx_comb].nb_tuiles - 1);
                int idx_tuile;
                scanf("%d", &idx_tuile);
                getchar();
                
                Tuile tuile_retiree = plateau->combinaisons[idx_comb].tuiles[idx_tuile];
                
                if (retirer_tuile_combinaison(&plateau->combinaisons[idx_comb], idx_tuile)) {
                    if (plateau_est_valide(plateau)) {
                        printf("✓ Tuile retirée. Vous devez la réutiliser immédiatement!\n");
                        ajouter_tuile_joueur(joueur, tuile_retiree);
                        a_pose_tuile = 1;
                        // Le joueur doit maintenant utiliser cette tuile
                        printf("Choisissez une action pour utiliser cette tuile (1=poser combinaison, 2=ajouter à combinaison): ");
                        int action;
                        scanf("%d", &action);
                        getchar();
                        // Simplifié - devrait être mieux géré
                    } else {
                        printf("❌ Cette action rend le plateau invalide\n");
                        // Restaurer
                        ajouter_tuile_combinaison(&plateau->combinaisons[idx_comb], tuile_retiree);
                    }
                } else {
                    printf("❌ Impossible de retirer cette tuile\n");
                }
                break;
            }
            
            case 4: { // Diviser une suite
                if (plateau->nb_combinaisons == 0) {
                    printf("❌ Aucune combinaison sur le plateau\n");
                    break;
                }
                printf("Index de la combinaison à diviser (0-%d): ", plateau->nb_combinaisons - 1);
                int idx_comb;
                scanf("%d", &idx_comb);
                getchar();
                
                if (idx_comb < 0 || idx_comb >= plateau->nb_combinaisons) {
                    printf("❌ Index invalide\n");
                    break;
                }
                
                if (plateau->combinaisons[idx_comb].type != 0) {
                    printf("❌ On ne peut diviser que des suites\n");
                    break;
                }
                
                if (a_joker(&plateau->combinaisons[idx_comb])) {
                    printf("❌ Impossible de diviser une combinaison contenant un joker\n");
                    break;
                }
                
                printf("Index où diviser (1-%d): ", plateau->combinaisons[idx_comb].nb_tuiles - 1);
                int idx_division;
                scanf("%d", &idx_division);
                getchar();
                
                Combinaison comb1, comb2;
                if (diviser_combinaison(plateau, idx_comb, idx_division, &comb1, &comb2)) {
                    retirer_combinaison_plateau(plateau, idx_comb);
                    ajouter_combinaison_plateau(plateau, &comb1);
                    ajouter_combinaison_plateau(plateau, &comb2);
                    printf("✓ Suite divisée en deux!\n");
                    a_pose_tuile = 1;
                } else {
                    printf("❌ Impossible de diviser cette combinaison\n");
                }
                break;
            }
            
            case 5: { // Remplacer tuile
                if (plateau->nb_combinaisons == 0) {
                    printf("❌ Aucune combinaison sur le plateau\n");
                    break;
                }
                printf("Index de la combinaison (0-%d): ", plateau->nb_combinaisons - 1);
                int idx_comb;
                scanf("%d", &idx_comb);
                getchar();
                
                printf("Index de la tuile dans la combinaison à remplacer: ");
                int idx_tuile_comb;
                scanf("%d", &idx_tuile_comb);
                getchar();
                
                printf("Index de la tuile dans votre chevalet: ");
                int idx_tuile_chevalet;
                scanf("%d", &idx_tuile_chevalet);
                getchar();
                
                if (idx_comb < 0 || idx_comb >= plateau->nb_combinaisons ||
                    idx_tuile_comb < 0 || idx_tuile_comb >= plateau->combinaisons[idx_comb].nb_tuiles ||
                    idx_tuile_chevalet < 0 || idx_tuile_chevalet >= joueur->nb_tuiles) {
                    printf("❌ Indices invalides\n");
                    break;
                }
                
                Tuile ancienne = plateau->combinaisons[idx_comb].tuiles[idx_tuile_comb];
                Tuile nouvelle = joueur->tuiles[idx_tuile_chevalet];
                
                if (remplacer_tuile_combinaison(&plateau->combinaisons[idx_comb], idx_tuile_comb, nouvelle)) {
                    if (plateau_est_valide(plateau)) {
                        retirer_tuile_joueur(joueur, idx_tuile_chevalet);
                        ajouter_tuile_joueur(joueur, ancienne);
                        printf("✓ Tuile remplacée!\n");
                        a_pose_tuile = 1;
                    } else {
                        printf("❌ Cette action rend le plateau invalide\n");
                        remplacer_tuile_combinaison(&plateau->combinaisons[idx_comb], idx_tuile_comb, ancienne);
                    }
                } else {
                    printf("❌ Impossible de remplacer cette tuile\n");
                }
                break;
            }
            
            case 6: { // Récupérer joker
                if (plateau->nb_combinaisons == 0) {
                    printf("❌ Aucune combinaison sur le plateau\n");
                    break;
                }
                
                // Trouver les jokers sur le plateau
                int jokers_trouves = 0;
                for (int i = 0; i < plateau->nb_combinaisons; i++) {
                    for (int j = 0; j < plateau->combinaisons[i].nb_tuiles; j++) {
                        if (plateau->combinaisons[i].tuiles[j].type == JOKER) {
                            printf("Joker trouvé: Combinaison %d, Tuile %d\n", i, j);
                            jokers_trouves++;
                        }
                    }
                }
                
                if (jokers_trouves == 0) {
                    printf("❌ Aucun joker sur le plateau\n");
                    break;
                }
                
                printf("Index de la combinaison contenant le joker: ");
                int idx_comb;
                scanf("%d", &idx_comb);
                getchar();
                
                printf("Index de la tuile joker dans la combinaison: ");
                int idx_joker;
                scanf("%d", &idx_joker);
                getchar();
                
                printf("Index de la tuile dans votre chevalet pour remplacer le joker: ");
                int idx_tuile;
                scanf("%d", &idx_tuile);
                getchar();
                
                if (idx_comb < 0 || idx_comb >= plateau->nb_combinaisons ||
                    idx_joker < 0 || idx_joker >= plateau->combinaisons[idx_comb].nb_tuiles ||
                    idx_tuile < 0 || idx_tuile >= joueur->nb_tuiles) {
                    printf("❌ Indices invalides\n");
                    break;
                }
                
                Tuile joker_recupere;
                if (recuperer_joker(plateau, idx_comb, idx_joker, joueur->tuiles[idx_tuile], &joker_recupere)) {
                    if (plateau_est_valide(plateau)) {
                        retirer_tuile_joueur(joueur, idx_tuile);
                        ajouter_tuile_joueur(joueur, joker_recupere);
                        printf("✓ Joker récupéré! Vous devez le rejouer immédiatement.\n");
                        a_pose_tuile = 1;
                        // Le joueur doit rejouer le joker
                        printf("Choisissez une action pour utiliser ce joker (1=poser combinaison, 2=ajouter à combinaison): ");
                        int action;
                        scanf("%d", &action);
                        getchar();
                        // Simplifié
                    } else {
                        printf("❌ Cette action rend le plateau invalide\n");
                        // Restaurer
                        remplacer_tuile_combinaison(&plateau->combinaisons[idx_comb], idx_joker, joker_recupere);
                    }
                } else {
                    printf("❌ Impossible de récupérer ce joker\n");
                }
                break;
            }
            
            case 7: // Piocher
                if (pioche->nb_tuiles > 0) {
                    Tuile tuile = piocher_tuile(pioche);
                    ajouter_tuile_joueur(joueur, tuile);
                    printf("✓ Vous avez pioché: ");
                    afficher_tuile_console(tuile);
                    printf("\n");
                    return 0; // Fin du tour
                } else {
                    printf("❌ La pioche est vide!\n");
                }
                break;
                
            case 8: // Voir chevalet
                afficher_chevalet_console(joueur);
                break;
                
            case 9: // Voir plateau
                afficher_plateau_console(plateau);
                break;
                
            case 10: // Info pioche
                printf("\n📦 Pioche: %d tuiles restantes\n", pioche->nb_tuiles);
                break;
                
            case 0: // Quitter
                return -1;
                
            default:
                printf("❌ Choix invalide. Réessayez.\n");
                break;
        }
        
        // Si le joueur a posé des tuiles, vérifier qu'il peut terminer son tour
        if (a_pose_tuile && !premier_coup) {
            printf("\nVoulez-vous continuer à jouer ? (1=Oui, 0=Non, terminer le tour): ");
            int continuer;
            scanf("%d", &continuer);
            getchar();
            if (!continuer) {
                if (plateau_est_valide(plateau)) {
                    return 1; // Tour terminé avec succès
                } else {
                    printf("❌ ERREUR: Le plateau est invalide! Vous devez corriger.\n");
                    *plateau = plateau_backup;
                    a_pose_tuile = 0;
                }
            }
        } else if (a_pose_tuile && premier_coup) {
            // Vérifier les 30 points
            int total = calculer_total_points_combinaisons(plateau->combinaisons, plateau->nb_combinaisons);
            if (total >= 30) {
                                printf("✓ Premier coup validé! Total: %d points\n", total);
                                etat->a_joue_premier_coup[index_joueur] = 1;
                if (plateau_est_valide(plateau)) {
                    return 1;
                }
            }
        }
    }
    
    return 0;
}

// Fonction pour gérer le tour d'un joueur IA
int tour_joueur_ia(Joueur *joueur, Plateau *plateau, Pioche *pioche, int premier_coup, EtatPremierCoup *etat, int index_joueur) {
    printf("\n🤖 Tour de l'IA: %s\n", joueur->pseudo);
    
    CoupIA coup;
    if (choisir_coup_ia(joueur, plateau, &coup)) {
        switch (coup.type_action) {
            case 0: // Poser combinaison
                if (ajouter_combinaison_plateau(plateau, &coup.nouvelle_combinaison)) {
                    printf("✓ L'IA a posé une combinaison\n");
                    // Retirer les tuiles du chevalet (simplifié)
                    return 1;
                }
                break;
            case 1: // Ajouter tuile
                if (ajouter_tuile_combinaison(&plateau->combinaisons[coup.index_comb_plateau], 
                                             joueur->tuiles[coup.index_tuile_chevalet])) {
                    retirer_tuile_joueur(joueur, coup.index_tuile_chevalet);
                    printf("✓ L'IA a ajouté une tuile à une combinaison\n");
                    return 1;
                }
                break;
            case 4: // Récupérer joker
                {
                    Tuile joker;
                    if (recuperer_joker(plateau, coup.index_comb_plateau, coup.index_tuile_combinaison,
                                      joueur->tuiles[coup.index_tuile_chevalet], &joker)) {
                        retirer_tuile_joueur(joueur, coup.index_tuile_chevalet);
                        ajouter_tuile_joueur(joueur, joker);
                        printf("✓ L'IA a récupéré un joker\n");
                        return 1;
                    }
                }
                break;
        }
    }
    
    // Si aucun coup possible, piocher
    if (pioche->nb_tuiles > 0) {
        Tuile tuile = piocher_tuile(pioche);
        ajouter_tuile_joueur(joueur, tuile);
        printf("✓ L'IA a pioché une tuile\n");
    }
    
    return 0;
}

// Fonction principale
int main(int argc, char *argv[]) {
    // Variables du jeu
    Joueur joueurs[NB_JOUEURS_MAX];
    Pioche pioche;
    Plateau plateau;
    EtatPremierCoup etat_premier_coup = {0};
    int nb_joueurs = 2;
    int joueur_actif = 0;
    int continuer = 1;
    int variante_timer = 0;
    
    printf("╔═══════════════════════════════════════╗\n");
    printf("║      JEU RUMMIKUB - VERSION CONSOLE   ║\n");
    printf("║      Règles complètes implémentées     ║\n");
    printf("╚═══════════════════════════════════════╝\n\n");
    
    // Demander le nombre de joueurs
    printf("Nombre de joueurs (2-4): ");
    scanf("%d", &nb_joueurs);
    getchar();
    
    if (nb_joueurs < 2 || nb_joueurs > 4) {
        nb_joueurs = 2;
        printf("Nombre de joueurs fixé à 2 (par défaut)\n");
    }
    
    // Demander si on active la variante timer
    printf("Activer la variante timer (1 minute par tour) ? (0=Non, 1=Oui): ");
    scanf("%d", &variante_timer);
    getchar();
    
    // Demander les pseudos
    demander_pseudos(joueurs, nb_joueurs);
    
    // Initialisation du jeu
    printf("\n=== Initialisation du jeu ===\n");
    initialiser_pioche(&pioche);
    creer_toutes_tuiles(&pioche);
    printf("✓ Pioche créée: %d tuiles\n", pioche.nb_tuiles);
    
    melanger_pioche(&pioche);
    printf("✓ Pioche mélangée\n");
    
    distribuer_tuiles(&pioche, joueurs, nb_joueurs);
    printf("✓ Tuiles distribuées (14 par joueur)\n");
    
    for (int i = 0; i < nb_joueurs; i++) {
        printf("  - %s: %d tuiles\n", joueurs[i].pseudo, joueurs[i].nb_tuiles);
    }
    
    joueur_actif = choisir_premier_joueur(joueurs, nb_joueurs, &pioche);
    printf("\n🎲 Le joueur %s commence!\n", joueurs[joueur_actif].pseudo);
    
    initialiser_plateau(&plateau);
    
    // Boucle principale du jeu
    printf("\n═══════════════════════════════════════════\n");
    printf("           DÉBUT DE LA PARTIE\n");
    printf("═══════════════════════════════════════════\n");
    
    while (continuer) {
        printf("\n");
        printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
        printf("🎮 TOUR DE %s\n", joueurs[joueur_actif].pseudo);
        printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
        
        int premier_coup = !etat_premier_coup.a_joue_premier_coup[joueur_actif];
        
        if (premier_coup) {
            printf("⚠️  PREMIER COUT: Vous devez poser au moins 30 points!\n");
            if (!peut_faire_30_points(&joueurs[joueur_actif])) {
                printf("❌ Vous ne pouvez pas faire 30 points. Vous devez piocher.\n");
                if (pioche.nb_tuiles > 0) {
                    Tuile tuile = piocher_tuile(&pioche);
                    ajouter_tuile_joueur(&joueurs[joueur_actif], tuile);
                    printf("✓ Vous avez pioché une tuile\n");
                }
                joueur_actif = (joueur_actif + 1) % nb_joueurs;
                continue;
            }
        }
        
        int resultat_tour;
        if (joueurs[joueur_actif].est_ia) {
            resultat_tour = tour_joueur_ia(&joueurs[joueur_actif], &plateau, &pioche, premier_coup, &etat_premier_coup, joueur_actif);
        } else {
            resultat_tour = tour_joueur_humain(&joueurs[joueur_actif], &plateau, &pioche, premier_coup, &etat_premier_coup, joueur_actif);
        }
        
        if (resultat_tour == -1) {
            // Quitter
            continuer = 0;
            break;
        }
        
        if (resultat_tour == 1) {
            etat_premier_coup.a_joue_premier_coup[joueur_actif] = 1;
        }
        
        // Vérifier si un joueur a gagné (plus de tuiles)
        for (int i = 0; i < nb_joueurs; i++) {
            if (joueurs[i].nb_tuiles == 0) {
                printf("\n🎉🎉🎉 %s A GAGNÉ ! 🎉🎉🎉\n", joueurs[i].pseudo);
                continuer = 0;
                break;
            }
        }
        
        if (continuer) {
            joueur_actif = (joueur_actif + 1) % nb_joueurs;
        }
        
        // Vérifier si la pioche est vide
        if (pioche.nb_tuiles == 0 && continuer) {
            printf("\n⚠️  La pioche est vide. La partie continue...\n");
            // Si personne n'a gagné et la pioche est vide, le gagnant est celui avec le moins de points
            int min_points = 1000;
            int gagnant = 0;
            for (int i = 0; i < nb_joueurs; i++) {
                int points = 0;
                for (int j = 0; j < joueurs[i].nb_tuiles; j++) {
                    if (joueurs[i].tuiles[j].type == JOKER) {
                        points += 30;
                    } else {
                        points += joueurs[i].tuiles[j].valeur;
                    }
                }
                if (points < min_points) {
                    min_points = points;
                    gagnant = i;
                }
            }
            printf("\n🎉🎉🎉 %s A GAGNÉ (moins de points: %d) ! 🎉🎉🎉\n", 
                   joueurs[gagnant].pseudo, min_points);
            continuer = 0;
        }
    }
    
    // Calculer et afficher les scores finaux
    printf("\n═══════════════════════════════════════════\n");
    printf("           SCORES FINAUX\n");
    printf("═══════════════════════════════════════════\n");
    
    // Trouver le gagnant
    int gagnant = -1;
    for (int i = 0; i < nb_joueurs; i++) {
        if (joueurs[i].nb_tuiles == 0) {
            gagnant = i;
            break;
        }
    }
    
    if (gagnant == -1) {
        // Gagnant par points (pioche vide)
        int min_points = 1000;
        for (int i = 0; i < nb_joueurs; i++) {
            int points = 0;
            for (int j = 0; j < joueurs[i].nb_tuiles; j++) {
                if (joueurs[i].tuiles[j].type == JOKER) {
                    points += 30;
                } else {
                    points += joueurs[i].tuiles[j].valeur;
                }
            }
            if (points < min_points) {
                min_points = points;
                gagnant = i;
            }
        }
    }
    
    // Calculer les scores
    int total_points_negatifs = 0;
    for (int i = 0; i < nb_joueurs; i++) {
        if (i == gagnant) {
            continue; // Le gagnant sera calculé après
        }
        
        int points_negatifs = 0;
        for (int j = 0; j < joueurs[i].nb_tuiles; j++) {
            if (joueurs[i].tuiles[j].type == JOKER) {
                points_negatifs += 30;
            } else {
                points_negatifs += joueurs[i].tuiles[j].valeur;
            }
        }
        joueurs[i].score = -points_negatifs;
        total_points_negatifs += points_negatifs;
        printf("%s: %d points (%d tuiles restantes)\n", 
               joueurs[i].pseudo, joueurs[i].score, joueurs[i].nb_tuiles);
    }
    
    // Score du gagnant
    if (gagnant >= 0) {
        joueurs[gagnant].score = total_points_negatifs;
        printf("%s (GAGNANT): %d points\n", 
               joueurs[gagnant].pseudo, joueurs[gagnant].score);
    }
    
    // Sauvegarder les scores
    printf("\n=== Sauvegarde des scores ===\n");
    for (int i = 0; i < nb_joueurs; i++) {
        if (!joueurs[i].est_ia) {
            sauvegarder_score(joueurs[i].pseudo, joueurs[i].score);
            printf("✓ Score de %s sauvegardé: %d points\n", 
                   joueurs[i].pseudo, joueurs[i].score);
        }
    }
    
    printf("\n═══════════════════════════════════════════\n");
    printf("        MERCI D'AVOIR JOUÉ !\n");
    printf("═══════════════════════════════════════════\n\n");
    
    return 0;
}
