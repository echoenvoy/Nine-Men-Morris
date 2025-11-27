#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "fonctions_de_jeu.h"
#include <ctype.h> 

//Start game
void startgame1(){
   int a = startgame0();
   if (a==11){
      joueurVsjoueur();}
   if (a==121){
      joueurVsMachine1();}
    if (a==122){
      joueurVsMachine3();}
   else if (a==2){
      rules();
      startgame1();}
   else if (a==3){
      printf(" Bye Bye !!");} 
}
//Definir un joueur aleatoire
int aleajoueur(){
    srand(time(NULL));
    int a=rand();
    return (a%2==0);
}

//choisir les symboles des joueurs 
void isValidCharch(char* sj){
   while(!isalpha(*sj)) {
        printf("Vous devez choisir une lettre : ");
        scanf(" %c",sj);
    }
}
//Vérifie si le symbole du joueur est différent du symbole de la machine.
void isDiffMachineAndBoard(char* sj){
  while (*sj == 'm' || *sj == 'O') {
        printf("C'est le symbole de la machine/ du plateau, choisissez un autre symbole : ");
        scanf(" %c", sj);
        // Clear the input buffer
        while (getchar() != '\n');
    } 
}
void choixSymboles(char *sj1,char *sj2,int a){
    clear();
    char sjd, sjg, sj;
    if (a==2){//Pour  joueur vs joueur
    printf("le joueur droite ,choisissez un symbole (un seul caractere):  ");
    scanf(" %c",&sjd);
    isValidCharch(&sjd);
    isDiffMachineAndBoard(&sjd);

    printf("le joueur gauche ,choisissez un symbole (un seul caractere):  ");
    scanf(" %c",&sjg);
    isValidCharch(&sjg);
    isDiffMachineAndBoard(&sjg);

    if (aleajoueur()==0){
        *sj1=sjd;*sj2=sjg;}
    else {
        *sj1=sjg;*sj2=sjd;
        }
   }
   if (a==1){//Pour un joueur vs la machine
    printf("le joueur  ,choisir un symbole (un seul caractere):  ");
    scanf(" %c",&sj);
    isValidCharch(&sj);
    isDiffMachineAndBoard(&sj);

    printf("Pour la machine, on choisit le symbole : 'm' \n");;
    if (aleajoueur()==0){
        printf("le joueur va commencer;  \n");
        *sj1=sj;*sj2='m';}
    else {
        printf("la machine va commencer ;  \n");
        *sj1='m';*sj2=sj;
        }
   }
}
//poser un pion dans la position i
void pose(int i,char x){
   board[i]=x;
   lastMove = i; 
}
//deplcer un pion d'une position i1 a une  position i2
void move(int i1, int i2, char x) {
    // Vérifier que la case d'origine contient bien le symbole du joueur
    if (board[i1] != x) {
        printf("Erreur : Tentative de déplacement d'un pion invalide.\n");
        return;
    }

    // Vérifier que la case de destination est bien vide
    if (board[i2] != 'O') {
        printf("Erreur : La case %d n'est pas vide !\n", i2);
        return;
    }


    // Déplacement du pion
    board[i1] = 'O';  // Vider l'ancienne case
    board[i2] = x;    // Placer le pion à la nouvelle position

    lastMove = i2; 
    displayBoard();
}


//verifie si le pion  est vide
int isValid(int i){
   return (board[i]=='O');
}


//Taking :
//Si un joueur peut a fait un moulin ,alors il prends un pion 

int checkAndHandleMoulin(int iplace, int *itake, char adversarySymbol, int *adversaryPawns) {
    if (moulin(iplace) ) {
        clear();
        printf(RED"Vous avez forme un moulin !"RESET);
        displayBoard();
        int hasNonMoulinPawns = 0;
        // Verifier s'il existe des pions adverses qui ne font pas partie d'un moulin
        for (int i = 0; i < SIZE; i++) {
            if (board[i] == adversarySymbol && !moulin(i)) {
                hasNonMoulinPawns = 1;
                break;
            }
        }

        // Demander au joueur de capturer un pion
        if (hasNonMoulinPawns) {
            printf("Choisissez un pion adverse a capturer (ne faisant pas partie d'un moulin) : ");
            scanf("%d", itake);
            // Verifier que le pion choisi est valide et ne fait pas partie d'un moulin
            while (!(board[*itake] == adversarySymbol && !moulin(*itake))) {
                printf("Position invalide. Veuillez choisir un pion adverse qui ne fait pas partie d'un moulin : ");
                scanf("%d", itake);
            }
        } else {
            printf("Tous les pions adverses font partie de moulins. Vous pouvez capturer n'importe lequel d'entre eux. Choisissez : ");
            scanf("%d", itake);

            // Verifier que le pion choisi appartient a l'adversaire
            while (!(board[*itake] == adversarySymbol)) {
                printf("Position invalide. Veuillez choisir un pion appartenant a l'adversaire : ");
                scanf("%d", itake);
            }
        }

        // Capturer le pion et mettre a jour les pions restants
        board[*itake] = 'O';
        (*adversaryPawns)--;
        return 1; // Moulin valide et pion capture
    }

    return 0; // Pas de moulin
}


void TourDePlacement(char joueurSymbol, char adversarySymbol, int joueurNum, int *adversaryPawns) {
    clear();
    printf(RED"C'est la phase de Placement :\n"RESET);
    displayBoard();
    int iplace, itake;
    int validInput;

    printf("Tour du joueur %d (%c)\n", joueurNum, joueurSymbol);

    do {
    printf(" Choisissez une position pour placer votre pion (0 a 23) :\n");
    validInput = scanf("%d", &iplace); // Tente de lire un entier
    
    if (validInput != 1) { // Si l'utilisateur entre une lettre ou un mauvais format
        printf("Erreur : Vous devez entrer un nombre !\n");
        while (getchar() != '\n'); // Vider le buffer d'entrée pour éviter une boucle infinie
        continue;
    } else if (!isValid(iplace) || iplace < 0 || iplace > 23) { // Vérifie si l'entrée est valide
        printf("Erreur : Position invalide. Veuillez choisir une position entre 0 et 23 !\n");
        validInput = 0; // Force la boucle à recommencer
    }

    } while (validInput != 1);

    // Placer le pion sur le plateau
    pose(iplace, joueurSymbol);

    // Verifier si un moulin a ete forme et gerer la capture
    if (checkAndHandleMoulin(iplace, &itake, adversarySymbol, adversaryPawns)) {
        printf("Le joueur %d a capture un pion adverse !\n", joueurNum);
    }
}

void displayArray(char arr[], int size) {
    for (int i = 0; i < size; i++) {
        printf("%c ", arr[i]);
    }
    printf("\n");
}

void TourDeMvt(char joueurSymbol, char adversarySymbol, int joueurNum,int joueurPawns, int *adversaryPawns) {
    clear();
    printf(RED"C'est la phase de Mouvement :\n"RESET);
    displayBoard();

    int ideplace, iaplace, itake, count,validInput;
    int valides[SIZE] = {-1}; 

    printf("Tour du joueur %d (%c)\n", joueurNum, joueurSymbol);

    if (joueurPawns == 3) {
        printf(YELLOW " Vous avez seulement 3 pions, vous pouvez sauter n'importe ou ! \n" RESET);
    }
    // Selection d'un pion a deplacer
    do {
        printf("Choisissez un pion a deplacer : ");
        validInput = scanf("%d", &ideplace); // Tente de lire un entier
    
        if (validInput != 1) { // Si l'utilisateur entre une lettre ou un mauvais format
           printf("Erreur : Vous devez entrer un nombre !\n");
           while (getchar() != '\n'); // Vider le buffer d'entrée pour éviter une boucle infinie
           continue;
        }

        // Vérifier que le pion appartient au joueur et peut bouger
        if (board[ideplace] != joueurSymbol) {
            printf("Ce n'est pas votre pion ! Essayez un autre.\n");
            continue;}

        count=isValidToMove(ideplace, valides,joueurPawns);
        if (!count) {
            printf("Ce pion ne peut pas etre deplace. Essayez un autre.\n");
            continue;}

    }  while (board[ideplace] != joueurSymbol || !count );

    // Affichage des positions valides
    printf("Positions disponibles pour le deplacement : ");
    for (int i = 0; valides[i] != -1 && i<count; i++) {
        printf("%d ", valides[i]);
        
    }
    printf("\n");

    // Selection de la nouvelle position
    do {
        printf("Choisissez une position parmi celles disponibles : ");
        validInput = scanf("%d", &iaplace);

        if (validInput != 1) { // Si l'utilisateur entre une lettre ou un mauvais format
           printf("Erreur : Vous devez entrer un nombre !\n");
           while (getchar() != '\n'); // Vider le buffer d'entrée pour éviter une boucle infinie
           continue;
        }
        if (!isInArray(iaplace, valides, count)) {
            printf("Choix invalide, veuillez reessayer.\n");
        }
    } while (!isInArray(iaplace, valides, count));

    // Effectuer le mouvement
    move(ideplace, iaplace, joueurSymbol);

    // Verifier et gerer un eventuel moulin
    checkAndHandleMoulin(iaplace, &itake, adversarySymbol, adversaryPawns);
}



//verifie si le pion peut bouger 
int isValidToMove(int ideplace,int* L,int joueurPawns ){
   int count = 0;
   for (int i = 0; i < SIZE; i++) {
    L[i] = -1;  // Réinitialiser toutes les positions
    }
    if (joueurPawns == 3) {
        // Déplacement libre : toutes les cases vides sont possibles
        for (int i = 0; i < SIZE; i++) {
            if (board[i] == 'O') {
                L[count++] = i;
            }
        }
    }else {
        // Déplacement normal : cases adjacentes seulement
        for (int j = 0; j < 4 && adjacences[ideplace][j] != -1; j++) {
            int voisin = adjacences[ideplace][j];
            if (board[voisin] == 'O') {
                L[count++] = voisin;
            }
        }
   return count;// Retourne 1 si au moins un mouvement est possible, sinon 0
}}

//apres une move ou un Placement cette fonction verifier si on a obtenu une moulin
int moulin(int i){
 int i1=i - i%3;
 //pour les moulins horizontales
 if ((board[i1] == board[i1+1]) && (board[i1] == board[i1+2])){
    return 1;}

//pour les moulins verticales 
 switch (i) {
 case 0:case 9 :case 21:
    return ((board[0] == board[9]) && (board[9] == board[21]));
 case 1:case 4 :case 7:
    return ((board[4] == board[1]) && (board[4] == board[7]));
 case 2:case 14 :case 23:
    return ((board[2] == board[14]) && (board[14] == board[23]));
 case 3:case 10 :case 18:
    return ((board[3] == board[10]) && (board[10] == board[18]));
 case 5:case 13 :case 20:
    return ((board[5] == board[13]) && (board[13] == board[20]));
 case 6:case 11 :case 15:
    return ((board[6] == board[11]) && (board[11] == board[15]));
 case 8:case 12 :case 17:
    return ((board[8] == board[12]) && (board[12] == board[17]));
 case 16:case 19 :case 22:
    return ((board[16] == board[19]) && (board[19] == board[22]));
 default :
   return 0;
}
}


int is_win() {
    if (nbrspions1 <= 2 || (isBlocked(sj1) && nbrspions1 != 3)) {//verifie si un joueur ne peut plus bouger ou si l'un des joueurs n’a que deux pions
        clear();
        displayBoard();
        if (sj2 == 'm'){
            printf(GREEN"La machine (%c) a gagne !\n"RESET,sj2);
        }
        else {
            printf(GREEN"Le deuxieme joueur (%c) a gagne !\n"RESET,sj2);
        }
        return 1; // Joueur 1 perd
    }
    if (nbrspions2 <= 2 || (isBlocked(sj2) && nbrspions2 != 3)) {//verifie si un joueur ne peut plus bouger ou si l'un des joueurs n’a que deux pions
        clear();
        displayBoard();
        if (sj1 == 'm'){
            printf(GREEN"La machine (%c) a gagne !\n"RESET,sj1);
        }
        else {
            printf(GREEN"Le premier joueur (%c) a gagne !\n"RESET,sj1);
        }
        
        return 1; // Joueur 2 perd
        
    }
    return 0; // Pas de victoire
}

int isBlocked(char sj) {
    for (int i = 0; i < SIZE; i++) {
        if (board[i] == sj) { // Trouver un pion du joueur
            int deplacement_possible = 0;

            // Vérifier toutes les positions adjacentes
            for (int j = 0; j < 4 && adjacences[i][j] != -1; j++) {
                int voisin = adjacences[i][j];
                if (board[voisin] == 'O') { // Vérifier si une case est libre
                    deplacement_possible = 1;
                    break; // Dès qu'un déplacement est possible, on peut sortir
                }
            }

            // Si un pion peut bouger, alors le joueur n'est pas bloqué
            if (deplacement_possible) return 0;
        }
    }
    return 1; // Aucun déplacement possible pour tous les pions du joueur
}


int isInArray(int a, int* L, int size) {
    for (int i = 0; i < size; i++) {
        if (L[i] == a) {
            return 1; // `a` is found in the array
        }
    }
    return 0; // `a` is not in the array
}
