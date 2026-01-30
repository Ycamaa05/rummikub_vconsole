#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "structures.h"
#include "jeu.h"
#include "graphique.h"
#include "fichiers.h"

#define NB_JOUEURS_MAX 4

// Fonction pour demander les pseudos des joueurs
void demander_pseudos(Joueur *joueurs, int nb_joueurs) {
    printf("=== Configuration des joueurs ===\n");
    for (int i = 0; i < nb_joueurs; i++) {
        char pseudo[50];
        printf("Joueur %d, entrez votre pseudo: ", i + 1);
        fgets(pseudo, sizeof(pseudo), stdin);
        // Enlever le saut de ligne
        pseudo[strcspn(pseudo, "\n")] = 0;
        
        if (strlen(pseudo) == 0) {
            sprintf(pseudo, "Joueur%d", i + 1);
        }
        
        initialiser_joueur(&joueurs[i], pseudo, 0);
        sauvegarder_pseudo(pseudo);
        printf("Joueur %d: %s\n", i + 1, pseudo);
    }
}

// Fonction principale
int main(int argc, char *argv[]) {
    // Variables du jeu
    Joueur joueurs[NB_JOUEURS_MAX];
    Pioche pioche;
    Plateau plateau;
    int nb_joueurs = 2; // Par défaut 2 joueurs
    int joueur_actif = 0;
    
    // Demander le nombre de joueurs
    printf("Nombre de joueurs (2-4): ");
    scanf("%d", &nb_joueurs);
    getchar(); // Consommer le \n
    
    if (nb_joueurs < 2 || nb_joueurs > 4) {
        nb_joueurs = 2;
    }
    
    // Demander les pseudos
    demander_pseudos(joueurs, nb_joueurs);
    
    // Initialisation du jeu
    printf("\n=== Initialisation du jeu ===\n");
    initialiser_pioche(&pioche);
    creer_toutes_tuiles(&pioche);
    printf("Pioche créée: %d tuiles\n", pioche.nb_tuiles);
    
    melanger_pioche(&pioche);
    printf("Pioche mélangée\n");
    
    distribuer_tuiles(&pioche, joueurs, nb_joueurs);
    printf("Tuiles distribuées (14 par joueur)\n");
    
    for (int i = 0; i < nb_joueurs; i++) {
        printf("Joueur %s: %d tuiles\n", joueurs[i].pseudo, joueurs[i].nb_tuiles);
    }
    
    joueur_actif = choisir_premier_joueur(joueurs, nb_joueurs, &pioche);
    printf("\nLe joueur %s commence!\n", joueurs[joueur_actif].pseudo);
    
    initialiser_plateau(&plateau);
    
    // Initialisation graphique
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    
    if (!init_graphique(&window, &renderer)) {
        printf("Erreur lors de l'initialisation graphique\n");
        return 1;
    }
    
    printf("\n=== Fenêtre graphique ouverte ===\n");
    printf("Contrôles:\n");
    printf("- Clic sur bouton 'Piocher' pour piocher une tuile\n");
    printf("- Clic sur bouton 'Passer' pour passer son tour\n");
    printf("- ESC pour quitter\n\n");
    
    // Boucle principale du jeu
    int continuer = 1;
    SDL_Event event;
    int bouton_piocher_survol = 0;
    int bouton_passer_survol = 0;
    
    const int bouton_x = 50;
    const int bouton_y = 700;
    const int bouton_w = 150;
    const int bouton_h = 50;
    
    while (continuer) {
        // Gestion des événements
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    continuer = 0;
                    break;
                    
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        continuer = 0;
                    }
                    break;
                    
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int x = event.button.x;
                        int y = event.button.y;
                        
                        // Bouton Piocher
                        if (est_clic_dans_rectangle(x, y, bouton_x, bouton_y, bouton_w, bouton_h)) {
                            if (pioche.nb_tuiles > 0) {
                                Tuile tuile = piocher_tuile(&pioche);
                                ajouter_tuile_joueur(&joueurs[joueur_actif], tuile);
                                printf("Joueur %s a pioché une tuile. Tuiles restantes: %d\n", 
                                       joueurs[joueur_actif].pseudo, pioche.nb_tuiles);
                                
                                // Passer au joueur suivant
                                joueur_actif = (joueur_actif + 1) % nb_joueurs;
                            } else {
                                printf("La pioche est vide!\n");
                            }
                        }
                        
                        // Bouton Passer
                        if (est_clic_dans_rectangle(x, y, bouton_x + bouton_w + 20, bouton_y, bouton_w, bouton_h)) {
                            printf("Joueur %s passe son tour\n", joueurs[joueur_actif].pseudo);
                            joueur_actif = (joueur_actif + 1) % nb_joueurs;
                        }
                    }
                    break;
                    
                case SDL_MOUSEMOTION:
                    {
                        int x = event.motion.x;
                        int y = event.motion.y;
                        bouton_piocher_survol = est_clic_dans_rectangle(x, y, bouton_x, bouton_y, bouton_w, bouton_h);
                        bouton_passer_survol = est_clic_dans_rectangle(x, y, bouton_x + bouton_w + 20, bouton_y, bouton_w, bouton_h);
                    }
                    break;
            }
        }
        
        // Affichage
        SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255); // Fond gris clair
        SDL_RenderClear(renderer);
        
        // Afficher le joueur actif
        char info_joueur[100];
        sprintf(info_joueur, "Tour de: %s", joueurs[joueur_actif].pseudo);
        afficher_texte(renderer, info_joueur, 50, 50, 20);
        
        // Afficher le chevalet du joueur actif
        afficher_chevalet(renderer, &joueurs[joueur_actif], 50, 100);
        
        // Afficher les autres joueurs (en plus petit)
        int y_autres = 200;
        for (int i = 0; i < nb_joueurs; i++) {
            if (i != joueur_actif) {
                afficher_chevalet(renderer, &joueurs[i], 50, y_autres);
                y_autres += 100;
            }
        }
        
        // Afficher le plateau
        afficher_plateau(renderer, &plateau, 50, 500);
        
        // Afficher les boutons
        afficher_bouton(renderer, "Piocher", bouton_x, bouton_y, bouton_w, bouton_h, bouton_piocher_survol);
        afficher_bouton(renderer, "Passer", bouton_x + bouton_w + 20, bouton_y, bouton_w, bouton_h, bouton_passer_survol);
        
        // Afficher les informations de la pioche
        char info_pioche[50];
        sprintf(info_pioche, "Pioche: %d tuiles", pioche.nb_tuiles);
        afficher_texte(renderer, info_pioche, 50, 30, 16);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }
    
    // Sauvegarder les scores (exemple avec scores fictifs)
    printf("\n=== Sauvegarde des scores ===\n");
    for (int i = 0; i < nb_joueurs; i++) {
        int score_fictif = 100 - (i * 20); // Score fictif pour démonstration
        sauvegarder_score(joueurs[i].pseudo, score_fictif);
        printf("Score de %s: %d points\n", joueurs[i].pseudo, score_fictif);
    }
    
    // Fermeture
    fermer_graphique(window, renderer);
    printf("\n=== Jeu terminé ===\n");
    
    return 0;
}

