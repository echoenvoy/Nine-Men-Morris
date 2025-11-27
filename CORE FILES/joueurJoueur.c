#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "fonctions_de_jeu.h"

void joueurVsjoueur(){
    choixSymboles(&sj1,&sj2,2);// Choix des symboles et détermination de l'ordre
    afficherCouleursJoueurs();
    Sleep(5000);
    //Phase de Placement
        for(int i=0;i<Pions/2;i++){
            
             //tour de premier joueur :
            if (nbrspions1 > 0) {
            TourDePlacement(sj1, sj2, 1, &nbrspions2);
        }

            //tour de deuxieme joueur :
            if (nbrspions2 > 0) {
            TourDePlacement(sj2, sj1, 2, &nbrspions1);
        }}


    //Phase de mouvement       
    printf(RED"Maintnant, c'est la phase de mouvement \n"RESET);
    while(!is_win()){

        //tour de premier joueur
            TourDeMvt(sj1,sj2,1,nbrspions1,&nbrspions2);
            if(is_win()) break;

        //tour de deuxieme joueur
            TourDeMvt(sj2,sj1,2,nbrspions2,&nbrspions1);
    }
    
}
int main(){
    startgame1();
    system("PAUSE");
    return 0;
}
