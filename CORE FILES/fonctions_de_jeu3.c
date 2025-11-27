#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "fonctions_de_jeu.h"
#include <ctype.h>

// Advanced AI placement with strategic decision making
void TourDePlacementMACHINE(char adversarySymbol, int *adversaryPawns) {
    clear();
    printf(RED"C'est la phase de Placement (Machine Avancee) :\n"RESET);
    displayBoard();
    
    int bestPosition = getBestMovePlacement(board, adversarySymbol);
    
    // Placer le pion sur le plateau
    pose(bestPosition, 'm');
    printf("La machine avancee a place un pion a la position %d\n", bestPosition);
    
    // Verifier si un moulin a ete forme et gerer la capture
    checkAndHandleMoulinMACHINE(bestPosition, adversarySymbol, adversaryPawns);
}

// Advanced AI movement with strategic decision making
void TourDeMvtMACHINE(char adversarySymbol, int *adversaryPawns) {
    clear();
    printf(RED"C'est la phase de Mouvement (Machine Avancee) :\n"RESET);
    displayBoard();
    
    Move bestMove = getBestMoveMvt(board, adversarySymbol);
    
    // Effectuer le mouvement
    move(bestMove.from, bestMove.to, 'm');
    printf("Machine avancee deplace son pion de %d a %d\n", bestMove.from, bestMove.to);
    Sleep(3000);
    
    // Verifier et gerer un eventuel moulin
    checkAndHandleMoulinMACHINE(bestMove.to, adversarySymbol, adversaryPawns);
}

// Advanced mill handling for AI
int checkAndHandleMoulinMACHINE(int iplace, char adversarySymbol, int *adversaryPawns) {
    if (moulin(iplace)) {
        clear();
        printf(RED"La machine avancee a forme un moulin !"RESET);
        displayBoard();
        
        int capturePosition;
        if (nbrspions1 > 3 && nbrspions2 > 3) {
            capturePosition = bestcapture1(board, adversarySymbol);
        } else {
            capturePosition = bestcapture0(board, adversarySymbol);
        }
        
        // Capturer le pion et mettre a jour les pions restants
        board[capturePosition] = 'O';
        (*adversaryPawns)--;
        printf("La machine avancee a capture le pion a la position %d\n", capturePosition);
        return 1;
    }
    return 0;
}

// Evaluate board state for minimax (simplified)
int evaluateBoard(char *board, char machineSymbol, char humanSymbol) {
    int score = 0;
    
    // Count mills
    for (int i = 0; i < SIZE; i++) {
        if (board[i] == machineSymbol && moulin(i)) {
            score += 100;
        }
        if (board[i] == humanSymbol && moulin(i)) {
            score -= 100;
        }
    }
    
    // Count pieces
    int machinePieces = 0, humanPieces = 0;
    for (int i = 0; i < SIZE; i++) {
        if (board[i] == machineSymbol) machinePieces++;
        if (board[i] == humanSymbol) humanPieces++;
    }
    score += (machinePieces - humanPieces) * 10;
    
    // Mobility evaluation
    int machineMobility = 0, humanMobility = 0;
    for (int i = 0; i < SIZE; i++) {
        if (board[i] == machineSymbol) {
            int moves[SIZE];
            machineMobility += isValidToMove(i, moves, machinePieces);
        }
        if (board[i] == humanSymbol) {
            int moves[SIZE];
            humanMobility += isValidToMove(i, moves, humanPieces);
        }
    }
    score += (machineMobility - humanMobility) * 2;
    
    return score;
}

// Get best placement position using heuristic evaluation
int getBestMovePlacement(char *board, char adversarySymbol) {
    int bestScore = -10000;
    int bestPosition = -1;
    
    for (int i = 0; i < SIZE; i++) {
        if (isValid(i)) {
            // Try this move
            board[i] = 'm';
            
            int score = 0;
            
            // Immediate mill formation
            if (moulin(i)) {
                score += 500;
                
                // Additional points if we can capture strategically
                score += evaluateCaptureValue(i, adversarySymbol);
            }
            
            // Threat creation (potential mills)
            score += threatPlacement(board, adversarySymbol, i) * 50;
            
            // Strategic positions (intersections, center of sides)
            if (isStrategicPosition(i)) {
                score += 30;
            }
            
            // Block opponent's potential mills
            char tempBoard[SIZE];
            memcpy(tempBoard, board, SIZE);
            tempBoard[i] = 'O'; // Reset
            tempBoard[i] = adversarySymbol;
            if (moulin(i)) {
                score += 100; // Blocking opponent's mill
            }
            
            // Future threat potential
            score += createsFutureThreat(i, adversarySymbol) * 20;
            
            // Reset board
            board[i] = 'O';
            
            if (score > bestScore) {
                bestScore = score;
                bestPosition = i;
            }
        }
    }
    
    // Fallback to random if no good move found
    if (bestPosition == -1) {
        do {
            bestPosition = rand() % SIZE;
        } while (!isValid(bestPosition));
    }
    
    return bestPosition;
}

// Get best movement using heuristic evaluation
Move getBestMoveMvt(char *board, char adversarySymbol) {
    Move bestMove = {-1, -1};
    int bestScore = -10000;
    int machinePieces = 0;
    
    for (int i = 0; i < SIZE; i++) {
        if (board[i] == 'm') {
            machinePieces++;
        }
    }
    
    for (int from = 0; from < SIZE; from++) {
        if (board[from] == 'm') {
            int validMoves[SIZE];
            int moveCount = isValidToMove(from, validMoves, machinePieces);
            
            for (int j = 0; j < moveCount && validMoves[j] != -1; j++) {
                int to = validMoves[j];
                
                // Try this move
                char originalFrom = board[from];
                char originalTo = board[to];
                
                board[from] = 'O';
                board[to] = 'm';
                
                int score = 0;
                
                // Immediate mill formation
                if (moulin(to)) {
                    score += 600;
                    
                    // Evaluate capture opportunity
                    score += evaluateCaptureValue(to, adversarySymbol);
                }
                
                // Threat creation
                score += threatMouvement(board, adversarySymbol, from, to) * 40;
                
                // Mobility improvement
                int newMobility = countMobility(to);
                int oldMobility = countMobility(from);
                score += (newMobility - oldMobility) * 5;
                
                // Strategic positioning
                if (isStrategicPosition(to)) {
                    score += 25;
                }
                
                // Double mill potential
                if (doubleMoulinPossible2(board, 'm', to)) {
                    score += 150;
                }
                
                // Block opponent
                score -= threatPlacement(board, adversarySymbol, to) * 30;
                
                // Board evaluation
                score += evaluateBoard(board, 'm', adversarySymbol) / 10;
                
                // Reset board
                board[from] = originalFrom;
                board[to] = originalTo;
                
                if (score > bestScore) {
                    bestScore = score;
                    bestMove.from = from;
                    bestMove.to = to;
                }
            }
        }
    }
    
    // Fallback to random if no good move found
    if (bestMove.from == -1) {
        for (int from = 0; from < SIZE; from++) {
            if (board[from] == 'm') {
                int validMoves[SIZE];
                int moveCount = isValidToMove(from, validMoves, machinePieces);
                
                if (moveCount > 0) {
                    bestMove.from = from;
                    bestMove.to = validMoves[rand() % moveCount];
                    break;
                }
            }
        }
    }
    
    return bestMove;
}

// Threat detection for placement
int threatPlacement(char *board, char adversarySymbol, int i) {
    int threats = 0;
    
    // Check horizontal threats
    int row = i - i % 3;
    for (int j = 0; j < 3; j++) {
        int pos = row + j;
        if (pos != i && board[pos] == 'm') {
            threats++;
        }
    }
    
    // Check vertical threats based on position
    switch (i) {
        case 0: case 9: case 21:
            if ((i == 0 && (board[9] == 'm' || board[21] == 'm')) ||
                (i == 9 && (board[0] == 'm' || board[21] == 'm')) ||
                (i == 21 && (board[0] == 'm' || board[9] == 'm'))) {
                threats++;
            }
            break;
        // Add other vertical cases similarly...
    }
    
    return threats;
}

// Threat detection for movement
int threatMouvement(char *board, char adversarySymbol, int source, int destination) {
    int threats = 0;
    
    // Check if move creates new threats
    board[source] = 'O';
    board[destination] = 'm';
    
    // Check all possible mills from new position
    for (int i = 0; i < SIZE; i++) {
        if (board[i] == 'O') {
            board[i] = 'm';
            if (moulin(i)) {
                threats++;
            }
            board[i] = 'O';
        }
    }
    
    // Reset board
    board[source] = 'm';
    board[destination] = 'O';
    
    return threats;
}

// Strategic position evaluation
int isStrategicPosition(int pos) {
    // Center positions are more strategic
    int strategicPositions[] = {4, 10, 13, 19};
    for (int i = 0; i < 4; i++) {
        if (pos == strategicPositions[i]) {
            return 1;
        }
    }
    return 0;
}

// Future threat potential
int createsFutureThreat(int pos, char adversarySymbol) {
    int threats = 0;
    
    // Check if this position could lead to double mills
    if (doubleMoulinPossible2(board, 'm', pos)) {
        threats += 2;
    }
    
    return threats;
}

// Evaluate capture value
int evaluateCaptureValue(int pos, char adversarySymbol) {
    int value = 0;
    
    // Prefer capturing pieces that are part of potential mills
    for (int i = 0; i < SIZE; i++) {
        if (board[i] == adversarySymbol) {
            // Check if this piece is blocking our potential mill
            char temp = board[i];
            board[i] = 'O';
            
            // Check if we can form a mill by placing at this position
            board[pos] = 'm';
            if (moulin(pos)) {
                value += 100;
            }
            board[pos] = 'O';
            
            board[i] = temp;
        }
    }
    
    return value;
}

// Count mobility for a position
int countMobility(int pos) {
    int mobility = 0;
    for (int j = 0; j < 4 && adjacences[pos][j] != -1; j++) {
        if (board[adjacences[pos][j]] == 'O') {
            mobility++;
        }
    }
    return mobility;
}

// Advanced capture logic for mid-game
int bestcapture1(char *board, char adversarySymbol) {
    int bestCapture = -1;
    int bestScore = -1000;
    
    for (int i = 0; i < SIZE; i++) {
        if (board[i] == adversarySymbol) {
            int score = 0;
            
            // Avoid breaking opponent's mills if possible
            if (!moulin(i)) {
                score += 200; // Prefer non-mill pieces
            }
            
            // Prefer pieces with high mobility
            score += countMobility(i) * 10;
            
            // Prefer pieces that are part of potential threats
            if (threatPlacement(board, 'm', i) > 0) {
                score += 150;
            }
            
            if (score > bestScore) {
                bestScore = score;
                bestCapture = i;
            }
        }
    }
    
    // Fallback
    if (bestCapture == -1) {
        for (int i = 0; i < SIZE; i++) {
            if (board[i] == adversarySymbol) {
                bestCapture = i;
                break;
            }
        }
    }
    
    return bestCapture;
}

// Advanced capture logic for end-game
int bestcapture0(char *board, char adversarySymbol) {
    int bestCapture = -1;
    int bestScore = -1000;
    
    for (int i = 0; i < SIZE; i++) {
        if (board[i] == adversarySymbol) {
            int score = 0;
            
            // In end-game, break mills to reduce opponent's options
            if (moulin(i)) {
                score += 300; // Prefer mill pieces to break formations
            }
            
            // Prefer pieces with low mobility (more isolated)
            score -= countMobility(i) * 5;
            
            if (score > bestScore) {
                bestScore = score;
                bestCapture = i;
            }
        }
    }
    
    // Fallback
    if (bestCapture == -1) {
        for (int i = 0; i < SIZE; i++) {
            if (board[i] == adversarySymbol) {
                bestCapture = i;
                break;
            }
        }
    }
    
    return bestCapture;
}

// Check for double mill possibility
int doubleMoulinPossible2(char *board, char symbol, int k) {
    int millCount = 0;
    
    // Store original
    char original = board[k];
    board[k] = symbol;
    
    // Check all possible mills involving this position
    if (moulin(k)) {
        millCount++;
    }
    
    // Check if position participates in multiple mill formations
    int row = k - k % 3;
    for (int i = 0; i < 3; i++) {
        int pos = row + i;
        if (pos != k && board[pos] == symbol) {
            millCount++;
        }
    }
    
    // Reset
    board[k] = original;
    
    return millCount >= 2;
}