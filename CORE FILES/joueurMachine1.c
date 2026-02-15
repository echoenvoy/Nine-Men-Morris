#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "fonctions_de_jeu.h"

void joueur_humain(char joueurSymbol, char adversarySymbol, int joueurNum, int joueurPawns, int *adversaryPawns, int phase) {
    if (phase == PHASE_PLACEMENT) {
        TourDePlacement(joueurSymbol, adversarySymbol, joueurNum, adversaryPawns);
        return;
    }
    TourDeMvt(joueurSymbol, adversarySymbol, joueurNum, joueurPawns, adversaryPawns);
}

void joueur_ordinateur(char machineSymbol, char adversarySymbol, int machinePawns, int *adversaryPawns, int phase, int difficulty) {
    if (phase == PHASE_PLACEMENT) {
        if (difficulty == 0) {
            Machine_placement(machineSymbol, adversarySymbol, adversaryPawns);
        } else {
            TourDePlacementMACHINE(adversarySymbol, adversaryPawns);
        }
        return;
    }
    if (difficulty == 0) {
        placementAleatoireMachine(machineSymbol, adversarySymbol, machinePawns, adversaryPawns);
    } else {
        TourDeMvtMACHINE(adversarySymbol, adversaryPawns);
    }
}

void joueurVsMachine1(){
    choixSymboles(&sj1, &sj2, 1);  // Choix des symboles et détermination de l'ordre
    afficherCouleursJoueurs();
    Sleep(5000);
    
    // Phase de placement
    for (int i = 0; i < Pions / 2; i++) {
        if (sj1 == 'm') {
            joueur_ordinateur(sj1, sj2, nbrspions1, &nbrspions2, PHASE_PLACEMENT, 0);
            joueur_humain(sj2, sj1, 2, nbrspions2, &nbrspions1, PHASE_PLACEMENT);
        } else {
            joueur_humain(sj1, sj2, 1, nbrspions1, &nbrspions2, PHASE_PLACEMENT);
            joueur_ordinateur(sj2, sj1, nbrspions2, &nbrspions1, PHASE_PLACEMENT, 0);
        }
    }

    // Phase de mouvement
    while (!is_win()) {
        printf("Maintenant, c'est la phase de mouvement \n");

        // Tour du 1 joueur ( humain ou machine )
        if (sj1 == 'm') {
            joueur_ordinateur(sj1, sj2, nbrspions1, &nbrspions2, PHASE_MOVEMENT, 0);
            if (is_win()) break;
        } else {
            joueur_humain(sj1, sj2, 1, nbrspions1, &nbrspions2, PHASE_MOVEMENT);
            if (is_win()) break;
        }

        // Tour de 2 joueur ( humain ou machine )
        if (sj2 == 'm') {
            joueur_ordinateur(sj2, sj1, nbrspions2, &nbrspions1, PHASE_MOVEMENT, 0);
        } else {
            joueur_humain(sj2, sj1, 2, nbrspions2, &nbrspions1, PHASE_MOVEMENT);
        }
    }
}

