
#ifndef FONCTIONS_DE_JEU_H
#define FONCTIONS_DE_JEU_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <wchar.h>
#include <limits.h> // For INT_MAX
#include <stdbool.h> // For bool type
#define RESET   "\033[0m"
#define RED     "\033[31m"  // Rouge
#define GREEN   "\033[32m"  // Vert
#define BLUE    "\033[34m"  // Bleu
#define YELLOW  "\033[33m"  // Jaune
#define MAGENTA "\033[35m"  // Magenta
#define CYAN    "\033[36m"  // Cyan
#define BBLACK   "\033[90m"  // Noir vif
#define BRED     "\033[91m"  // Rouge vif
#define BGREEN   "\033[92m"  // Vert vif
#define BYELLOW  "\033[93m"  // Jaune vif
#define BBLUE    "\033[94m"  // Bleu vif
#define BMAGENTA "\033[95m"  // Magenta vif
#define BCYAN    "\033[96m"  // Cyan vif
#define BOLD      "\033[1m"  // Gras
#define UNDERLINE "\033[4m"  // Souligné
#define REVERSE   "\033[7m"  // Inversion des couleurs (texte <-> fond)
#define HIDDEN    "\033[8m"  // Texte caché


#define SIZE 24
#define Pions 18 //9 pour chaque joueur 

typedef struct {
    int from;
    int to;
} Move;

// Déclaration des variables globales
extern int nbrspions1;
extern int nbrspions2;
extern char board[SIZE];
extern int adjacences[SIZE][4];
extern int lastMove; // Dernière case jouée
extern int moulinsPrecedents[SIZE];
extern int b; //variable globale pour choisir le joueur qui commence
extern int MoulinAssureTableau[2];
extern int c; // pour savoir si il y a possibilite d'un moulin assure pour l'adversaire



extern char sj1;
extern char sj2;

// Fonctions du jeu
void clear();
void afficherCouleursJoueurs();
void joueurVsjoueur();
void joueurVsMachine1();
void initializeBoard();
char rules();
void displayBoard0();
void displayBoard();
int startgame0();
void startgame1();
void choixSymboles1(char *sj1);
int aleajoueur();
void choixSymboles(char *sj1, char *sj2, int a);
void choixSymboles1(char *sj1);
void isValidCharch(char* sj);
void pose(int i, char x);
void move(int i1, int i2, char x);
int isValid(int i);
int isValidToTake(int iplace,int* itake,char sj);
void take(int i);
int checkAndHandleMoulin(int iplace, int *itake, char adversarySymbol, int *adversaryPawns);
void TourDePlacement(char joueurSymbol, char adversarySymbol, int joueurNum, int *adversaryPawns) ;
void TourDeMvt(char joueurSymbol, char adversarySymbol, int joueurNum,int joueurPawns, int *adversaryPawns) ;
int isValidToMove(int ideplace,int* L,int joueurPawns);
int moulin(int i);
int is_win();
int isBlocked(char sj);
int isInArray(int a, int* L, int size);
void Machine_placement(char smachine,char shumain, int *humanPawns) ;
void placementAleatoireMachine(char smachine,char shumain,int Machine_Pawns, int *adversaryPawns);
void joueur_humain();
void joueur_ordinateur();


// News :
void joueurVsMachine3();
void startAdvancedAIGame();
void TourDePlacementMACHINE(char adversarySymbol, int *adversaryPawns);
void TourDeMvtMACHINE(char adversarySymbol, int *adversaryPawns);
int checkAndHandleMoulinMACHINE(int iplace, char adversarySymbol, int *adversaryPawns);
int evaluateBoard(char *board, char machineSymbol, char humanSymbol);
int getBestMovePlacement(char *board, char adversarySymbol);
Move getBestMoveMvt(char *board, char adversarySymbol);
int threatPlacement(char *board, char adversarySymbol, int i);
int threatMouvement(char *board, char adversarySymbol, int source, int destination);
int isStrategicPosition(int pos);
int createsFutureThreat(int pos, char adversarySymbol);
int evaluateCaptureValue(int pos, char adversarySymbol);
int countMobility(int pos);
int bestcapture1(char *board, char adversarySymbol);
int bestcapture0(char *board, char adversarySymbol);
int doubleMoulinPossible2(char *board, char symbol, int k);
#endif
