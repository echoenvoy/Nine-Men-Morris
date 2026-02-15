#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include "fonctions_de_jeu.h"
int nbrspions1=9,nbrspions2=9;
char sj1, sj2;
int lastMove = -1; // -1 signifie qu'aucun coup n'a encore été joué
int moulinsPrecedents[SIZE] = {0};  // 0 = Pas encore formé, 1 = Déjà compté
int allowFlying = 1;
int aiSearchDepth = 0;

static int readIntChoice(const char *prompt, const char *errorMsg, int min, int max) {
    char buffer[64];

    for (;;) {
        char *end = NULL;
        long value = 0;

        if (prompt && *prompt) {
            printf("%s", prompt);
        }

        if (!fgets(buffer, sizeof(buffer), stdin)) {
            return min;
        }

        value = strtol(buffer, &end, 10);
        if (end == buffer) {
            printf("%s", errorMsg);
            continue;
        }

        while (*end != '\0' && isspace((unsigned char)*end)) {
            end++;
        }

        if (*end != '\0') {
            printf("%s", errorMsg);
            continue;
        }

        if (value < min || value > max) {
            printf("%s", errorMsg);
            continue;
        }

        return (int)value;
    }
}

//Pour effacer les sorties précédentes lors de l'exécution 
void clear(){
    system("cls");
}

// affiche les couleurs des joueurs avant le début du jeu.
void afficherCouleursJoueurs() {
    printf("\n");
    SetConsoleOutputCP(CP_UTF8);
    printf("******************************************\n");
    printf("          COULEURS DES JOUEURS            \n");
    printf("******************************************\n");
    wprintf(L"👤 Joueur 1 : \033[31mROUGE\033[0m (%c)\n", sj1);
    wprintf(L"👤 Joueur 2 : \033[32mVERT\033[0m (%c)\n", sj2);
    wprintf(L"⭐ Dernier coup joue : \033[33mJAUNE\033[0m\n");
    printf("******************************************\n\n");
}


// Fonction pour afficher une case avec couleur
void printColoredChar(char c, char joueurSymbol1, char joueurSymbol2, int position) {
    if (position == lastMove) {
        printf("\033[33m%c\033[0m", c); // JAUNE et entre [ ] pour surligner
    } else if (c == joueurSymbol1) {
        printf("\033[31m%c\033[0m", c); // Rouge pour Joueur 1
    } else if (c == joueurSymbol2) {
        printf("\033[32m%c\033[0m", c); // Vert pour Joueur 2
    } else {
        printf("%c", c); // Normal
    }
}


// Representation du plateau de jeu
char board[SIZE] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};


int adjacences[SIZE][4] = {
    {1, 9, -1, -1},   // Case 0
    {0, 2, 4, -1},    // Case 1
    {1, 14, -1, -1},  // Case 2
    {4, 10, -1, -1},  // Case 3
    {1, 3, 5, 7},     // Case 4
    {4, 13, -1, -1},  // Case 5
    {7, 11, -1, -1},  // Case 6
    {4, 6, 8, -1},    // Case 7
    {7, 12, -1, -1},  // Case 8
    {0, 10, 21, -1},  // Case 9
    {3, 9, 11, 18},   // Case 10
    {6, 10, 15, -1},  // Case 11
    {8, 13, 17, -1},  // Case 12
    {5, 12, 14, 20},  // Case 13
    {2, 13, 23, -1},  // Case 14
    {11, 16, -1, -1}, // Case 15
    {15, 17, 19, -1}, // Case 16
    {12, 16, -1, -1}, // Case 17
    {10, 19, -1, -1}, // Case 18
    {16, 18, 20, 22}, // Case 19
    {13, 19, -1, -1}, // Case 20
    {9, 22, -1, -1},  // Case 21
    {19, 21, 23, -1}, // Case 22
    {14, 22, -1, -1}  // Case 23
};

// Fonction pour initialiser le plateau de jeu
void initializeBoard() {
    for(int i = 0; i < SIZE; i++) {
        board[i] = 'O';}
}
/*void nbrPions(char x,char y,int *A, int *B){
 *A=0,*B=0;
   for(int i = 0; i < SIZE; i++){
      if(board[i]==x)
         (*A)++;
      if(board[i]==y)
         (*B)++;
   }}*/
char rules(){
    printf("Regles du Jeu : (Voir la documentation en annexe) \n"
           "1. Le jeu se deroule en deux temps : la pose puis le mouvement.\n"
           "2. Dans un premier temps, les joueurs posent a tour de rôle chacun de leurs\n"
           "   pions sur un point vide (intersection) du plateau.\n"
           "   - Le premier joueur est defini aleatoirement par la machine.\n"
           "   - Lorsqu’il n’y a plus de pion a poser, les joueurs deplacent leurs pions vers\n"
           "     une intersection voisine libre en suivant un chemin prevu. L’objectif est de\n"
           "     former des « moulins » ou files de trois pions alignes.\n"
           "   - Lorsque l'un des joueurs forme un moulin, il peut capturer un pion adverse, \n"
           "     il choisit librement le pion qu´il capturera entre tous les pions de l´adversaire\n"
           "     qui n´appartiennent a aucun moulin. Au cas où tous les pions de l´adversaire forment des moulins,\n" 
           "     celui qui fait la prise choisira librement un quelconque d´entre eux.\n"
           "3. Une fois sorti du jeu, un pion ne peut plus y rentrer.\n"
           "4. Le jeu s’acheve quand un joueur n’a plus que deux pions ou ne peut plus jouer,\n"
           "   il est alors le perdant.\n");
    return 0;
}
// Fonction pour afficher le plateau de jeu numérotée pour aider les joueurs à choisir les places 
void displayBoard0() {
    printf("\n");
    printf("0--------1--------2  \n");
    printf("|        |        |  \n");
    printf("|  3-----4-----5  |  \n");
    printf("|  |     |     |  |  \n");
    printf("|  |  6--7--8  |  |  \n");
    printf("|  |  |     |  |  |  \n");
    printf("9--10-11    12-13-14 \n");
    printf("|  |  |     |  |  |  \n");
    printf("|  |  15-16-17 |  |  \n");
    printf("|  |     |     |  |  \n");
    printf("|  18----19----20 |  \n");
    printf("|        |        |  \n");
    printf("21-------22-------23 \n");
}


// Fonction pour afficher le plateau de jeu 
void displayBoard() {
    displayBoard0();
    int ligne = 1;
    for(int i = 0; i < SIZE; i+=3) {
        if (i == 12) continue;

        if ((ligne == 1) || (ligne == 7)) {
            printColoredChar(board[i],sj1,sj2,i); printf("--------");
            printColoredChar(board[i+1],sj1,sj2,i+1); printf("--------");
            printColoredChar(board[i+2],sj1,sj2,i+2); printf("\n");
            if (ligne == 1) printf("|        |        |\n");
        }
        if ((ligne == 2) || (ligne == 6)) {
            printf("|  "); printColoredChar(board[i],sj1,sj2,i); printf("-----");
            printColoredChar(board[i+1],sj1,sj2,i+1); printf("-----");
            printColoredChar(board[i+2],sj1,sj2,i+2); printf("  |\n");
            if (ligne == 2) 
                printf("|  |     |     |  |\n");
            else 
                printf("|        |        |\n");
        }
        if ((ligne == 3) || (ligne == 5)) {
            printf("|  |  "); printColoredChar(board[i],sj1,sj2,i); printf("--");
            printColoredChar(board[i+1],sj1,sj2,i+1); printf("--");
            printColoredChar(board[i+2],sj1,sj2,i+2); printf("  |  |\n");
            if (ligne == 3) 
                printf("|  |  |     |  |  |\n");
            else 
                printf("|  |     |     |  |\n");
        }
        if (ligne == 4) {
            printColoredChar(board[i],sj1,sj2,i); printf("--");
            printColoredChar(board[i+1],sj1,sj2,i+1); printf("--");
            printColoredChar(board[i+2],sj1,sj2,i+2); printf("     ");
            printColoredChar(board[i+3],sj1,sj2,i+3); printf("--");
            printColoredChar(board[i+4],sj1,sj2,i+4); printf("--");
            printColoredChar(board[i+5],sj1,sj2,i+5); printf("\n");
            printf("|  |  |     |  |  |\n");
        }
        ligne++;
    }
}



int startgame0(){
    int choix;int choix2;
    int flyingChoice;
    int depthChoice;
     printf(GREEN "*****  JEU MOULIN  *****\n" RESET);
    printf(BLUE "Selectionnez une option :\n" RESET);
    printf(RED "  ***     Menu     ***  \n" RESET);
    printf("      1- JOUER \n");
    printf("      2- Regles \n");
    printf("      3- Quitter \n");
    printf("Choisir une option (1 ou 2 ou 3 ) \n");
    choix = readIntChoice(NULL, "Choix invalid,refaire le choix  (1 ou 2 ou 3 ) \n", 1, 3);
    if (choix==1){
        printf("      1- joueur vs joueur \n");
        printf("      2- joueur vs machine \n");
        printf("Choisir une option (1 ou 2) \n");
        choix2 = readIntChoice(NULL, "Choix invalid,refaire le choix  (1 ou 2 ) \n", 1, 2);
        printf("Activer la regle de saut (3 pions) ?\n");
        printf("      1- Oui\n");
        printf("      2- Non\n");
        flyingChoice = readIntChoice(NULL, "Choix invalid,refaire le choix  (1 ou 2 ) \n", 1, 2);
        allowFlying = (flyingChoice == 1);
        initializeBoard();
        if  (choix2==1){
            aiSearchDepth = 0;
            return 11 ;}
        else if  (choix2==2){
            printf("Choisir la difficulte (1- Debutant, 2- Moyen, 3- Difficile) \n");
            depthChoice = readIntChoice(NULL, "Choix invalid,refaire le choix  (1 a 3) \n", 1, 3);
            if (depthChoice == 1) {
                aiSearchDepth = 0;
                return 121 ;
            }
            aiSearchDepth = depthChoice - 1;
            return 122 ;
            }
        }
    else {
        return choix;}
    return -1;

}

