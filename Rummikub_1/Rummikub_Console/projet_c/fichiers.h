#ifndef FICHIERS_H
#define FICHIERS_H

// Sauvegarde des pseudos
int sauvegarder_pseudo(const char *pseudo);

// Sauvegarde des scores
int sauvegarder_score(const char *pseudo, int score);

// Charger les pseudos (optionnel)
int charger_pseudos(char pseudos[][50], int max_pseudos);

#endif // FICHIERS_H

