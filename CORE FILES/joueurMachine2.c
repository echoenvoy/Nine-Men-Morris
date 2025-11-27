#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "fonctions_de_jeu.h"

void joueurVsMachine3(){
    choixSymboles(&sj1, &sj2, 1);  // Choix des symboles et détermination de l'ordre
    afficherCouleursJoueurs();
    Sleep(5000);
    
    // Phase de placement
    for (int i = 0; i < Pions / 2; i++) {
        if (sj1 == 'm') {
            TourDePlacementMACHINE(sj2, &nbrspions2);  // Placement avancé de la machine
            TourDePlacement(sj2, sj1, 2, &nbrspions1);  // Placement du joueur
        } else {
            TourDePlacement(sj1, sj2, 1, &nbrspions2);  // Placement du joueur
            TourDePlacementMACHINE(sj1, &nbrspions1);  // Placement avancé de la machine
        }
    }

    // Phase de mouvement
    while (!is_win()) {
        printf("Maintenant, c'est la phase de mouvement (Machine Avancee) \n");

        // Tour du 1 joueur ( humain ou machine )
        if (sj1 == 'm') {
            TourDeMvtMACHINE(sj2, &nbrspions2);  // Mouvement avancé de la machine
            if (is_win()) break;
        } else {
            TourDeMvt(sj1, sj2, 1, nbrspions1, &nbrspions2);  // Tour de mouvement du joueur
            if (is_win()) break;
        }

        // Tour de 2 joueur ( humain ou machine )
        if (sj2 == 'm') {
            TourDeMvtMACHINE(sj1, &nbrspions1);  // Mouvement avancé de la machine
        } else {
            TourDeMvt(sj2, sj1, 2, nbrspions2, &nbrspions1);  // Tour de mouvement du joueur
        }
    }
}

// Function to be called from main menu for advanced AI
void startAdvancedAIGame() {
    initializeBoard();
    joueurVsMachine3();
}