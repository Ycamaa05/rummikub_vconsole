#include "fichiers.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

// Sauvegarder un pseudo dans un fichier
int sauvegarder_pseudo(const char *pseudo) {
    FILE *fichier = fopen("pseudos.txt", "a");
    if (fichier == NULL) {
        return 0;
    }
    
    fprintf(fichier, "%s\n", pseudo);
    fclose(fichier);
    return 1;
}

// Sauvegarder un score dans un fichier
int sauvegarder_score(const char *pseudo, int score) {
    FILE *fichier = fopen("scores.txt", "a");
    if (fichier == NULL) {
        return 0;
    }
    
    time_t maintenant = time(NULL);
    struct tm *temps = localtime(&maintenant);
    
    fprintf(fichier, "%s : %d points (Date: %02d/%02d/%04d %02d:%02d)\n", 
            pseudo, score, 
            temps->tm_mday, temps->tm_mon + 1, temps->tm_year + 1900,
            temps->tm_hour, temps->tm_min);
    
    fclose(fichier);
    return 1;
}

// Charger les pseudos depuis le fichier (optionnel)
int charger_pseudos(char pseudos[][50], int max_pseudos) {
    FILE *fichier = fopen("pseudos.txt", "r");
    if (fichier == NULL) {
        return 0;
    }
    
    int count = 0;
    char ligne[50];
    while (count < max_pseudos && fgets(ligne, sizeof(ligne), fichier)) {
        // Enlever le saut de ligne
        ligne[strcspn(ligne, "\n")] = 0;
        if (strlen(ligne) > 0) {
            strncpy(pseudos[count], ligne, 49);
            pseudos[count][49] = '\0';
            count++;
        }
    }
    
    fclose(fichier);
    return count;
}

