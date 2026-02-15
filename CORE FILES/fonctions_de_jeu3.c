#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "fonctions_de_jeu.h"
#include <ctype.h>

static int countMillsForSymbol(const char *b, char symbol);
static int countTwoInRowLines(const char *b, char symbol);
static int clampSearchDepth(int depth);

static int moulinOnBoard(const char *b, int i) {
    int i1 = i - i % 3;
    if ((b[i1] == b[i1 + 1]) && (b[i1] == b[i1 + 2])) {
        return 1;
    }

    switch (i) {
        case 0: case 9: case 21:
            return ((b[0] == b[9]) && (b[9] == b[21]));
        case 1: case 4: case 7:
            return ((b[4] == b[1]) && (b[4] == b[7]));
        case 2: case 14: case 23:
            return ((b[2] == b[14]) && (b[14] == b[23]));
        case 3: case 10: case 18:
            return ((b[3] == b[10]) && (b[10] == b[18]));
        case 5: case 13: case 20:
            return ((b[5] == b[13]) && (b[13] == b[20]));
        case 6: case 11: case 15:
            return ((b[6] == b[11]) && (b[11] == b[15]));
        case 8: case 12: case 17:
            return ((b[8] == b[12]) && (b[12] == b[17]));
        case 16: case 19: case 22:
            return ((b[16] == b[19]) && (b[19] == b[22]));
        default:
            return 0;
    }
}

static int countPiecesOnBoard(const char *b, char symbol) {
    int count = 0;
    for (int i = 0; i < SIZE; i++) {
        if (b[i] == symbol) {
            count++;
        }
    }
    return count;
}

static int hasEmptyOnBoard(const char *b) {
    for (int i = 0; i < SIZE; i++) {
        if (b[i] == 'O') {
            return 1;
        }
    }
    return 0;
}

static int canFormMillAt(char *b, int pos, char symbol) {
    if (b[pos] != 'O') {
        return 0;
    }
    char original = b[pos];
    b[pos] = symbol;
    int isMill = moulinOnBoard(b, pos);
    b[pos] = original;
    return isMill;
}

static int countImmediateMillMoves(char *b, char symbol) {
    int count = 0;
    for (int i = 0; i < SIZE; i++) {
        if (canFormMillAt(b, i, symbol)) {
            count++;
        }
    }
    return count;
}

static int countMobilityForSymbol(const char *b, char symbol) {
    int mobility = 0;
    for (int i = 0; i < SIZE; i++) {
        if (b[i] != symbol) {
            continue;
        }
        for (int j = 0; j < 4 && adjacences[i][j] != -1; j++) {
            int voisin = adjacences[i][j];
            if (b[voisin] == 'O') {
                mobility++;
            }
        }
    }
    return mobility;
}

static int evaluateCaptureImpact(char *b, int pos, char adversarySymbol) {
    int millsBefore = countMillsForSymbol(b, adversarySymbol);
    int twoBefore = countTwoInRowLines(b, adversarySymbol);
    int mobilityBefore = countMobilityForSymbol(b, adversarySymbol);
    int isMillPiece = moulinOnBoard(b, pos);
    int score = 0;
    char original = b[pos];

    b[pos] = 'O';
    int millsAfter = countMillsForSymbol(b, adversarySymbol);
    int twoAfter = countTwoInRowLines(b, adversarySymbol);
    int mobilityAfter = countMobilityForSymbol(b, adversarySymbol);

    if (millsAfter < millsBefore) {
        score += (millsBefore - millsAfter) * 450;
    }
    if (twoAfter < twoBefore) {
        score += (twoBefore - twoAfter) * 160;
    }
    if (mobilityAfter < mobilityBefore) {
        score += (mobilityBefore - mobilityAfter) * 8;
    }
    if (isMillPiece) {
        score += 120;
    }

    b[pos] = original;
    return score;
}

static int isBlockedOnBoard(const char *b, char symbol, int pieceCount) {
    if (allowFlying && pieceCount == 3) {
        return !hasEmptyOnBoard(b);
    }
    for (int i = 0; i < SIZE; i++) {
        if (b[i] == symbol) {
            for (int j = 0; j < 4 && adjacences[i][j] != -1; j++) {
                int voisin = adjacences[i][j];
                if (b[voisin] == 'O') {
                    return 0;
                }
            }
        }
    }
    return 1;
}

static int terminalScore(const char *b, char machineSymbol, char humanSymbol) {
    int machinePieces = countPiecesOnBoard(b, machineSymbol);
    int humanPieces = countPiecesOnBoard(b, humanSymbol);

    if (humanPieces <= 2 || isBlockedOnBoard(b, humanSymbol, humanPieces)) {
        return 100000;
    }
    if (machinePieces <= 2 || isBlockedOnBoard(b, machineSymbol, machinePieces)) {
        return -100000;
    }
    return 0;
}

static int minimaxPlacement(char *b, char currentSymbol, char otherSymbol, int depth, char machineSymbol, char humanSymbol) {
    int terminal = terminalScore(b, machineSymbol, humanSymbol);
    if (terminal != 0) {
        return terminal;
    }
    if (depth == 0) {
        return evaluateBoard(b, machineSymbol, humanSymbol);
    }

    int bestScore = (currentSymbol == machineSymbol) ? -1000000 : 1000000;
    int moved = 0;

    for (int i = 0; i < SIZE; i++) {
        if (b[i] == 'O') {
            moved = 1;
            b[i] = currentSymbol;
            int score = minimaxPlacement(b, otherSymbol, currentSymbol, depth - 1, machineSymbol, humanSymbol);
            b[i] = 'O';

            if (currentSymbol == machineSymbol) {
                if (score > bestScore) {
                    bestScore = score;
                }
            } else {
                if (score < bestScore) {
                    bestScore = score;
                }
            }
        }
    }

    if (!moved) {
        return evaluateBoard(b, machineSymbol, humanSymbol);
    }

    return bestScore;
}

static int getValidMovesForPiece(const char *b, int from, int *moves, int pieceCount) {
    int count = 0;
    if (allowFlying && pieceCount == 3) {
        for (int i = 0; i < SIZE; i++) {
            if (b[i] == 'O') {
                moves[count++] = i;
            }
        }
        return count;
    }

    for (int j = 0; j < 4 && adjacences[from][j] != -1; j++) {
        int voisin = adjacences[from][j];
        if (b[voisin] == 'O') {
            moves[count++] = voisin;
        }
    }
    return count;
}

static int minimaxMovement(char *b, char currentSymbol, char otherSymbol, int depth, char machineSymbol, char humanSymbol) {
    int terminal = terminalScore(b, machineSymbol, humanSymbol);
    if (terminal != 0) {
        return terminal;
    }
    if (depth == 0) {
        return evaluateBoard(b, machineSymbol, humanSymbol);
    }

    int bestScore = (currentSymbol == machineSymbol) ? -1000000 : 1000000;
    int moved = 0;
    int pieceCount = countPiecesOnBoard(b, currentSymbol);

    for (int from = 0; from < SIZE; from++) {
        if (b[from] != currentSymbol) {
            continue;
        }
        int moves[SIZE];
        int moveCount = getValidMovesForPiece(b, from, moves, pieceCount);
        for (int j = 0; j < moveCount; j++) {
            int to = moves[j];
            moved = 1;
            char originalFrom = b[from];
            char originalTo = b[to];
            b[from] = 'O';
            b[to] = currentSymbol;

            int score = minimaxMovement(b, otherSymbol, currentSymbol, depth - 1, machineSymbol, humanSymbol);

            b[from] = originalFrom;
            b[to] = originalTo;

            if (currentSymbol == machineSymbol) {
                if (score > bestScore) {
                    bestScore = score;
                }
            } else {
                if (score < bestScore) {
                    bestScore = score;
                }
            }
        }
    }

    if (!moved) {
        return evaluateBoard(b, machineSymbol, humanSymbol);
    }

    return bestScore;
}

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
        if (board[i] == machineSymbol && moulinOnBoard(board, i)) {
            score += 100;
        }
        if (board[i] == humanSymbol && moulinOnBoard(board, i)) {
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
    int oppThreatsBefore = countImmediateMillMoves(board, adversarySymbol);
    int oppMobilityBefore = countMobilityForSymbol(board, adversarySymbol);
    int aiThreatsBefore = countImmediateMillMoves(board, 'm');
    int twoOppBefore = countTwoInRowLines(board, adversarySymbol);
    int twoAiBefore = countTwoInRowLines(board, 'm');
    int minOppThreatsAfter = 100000;
    int blockPositions[SIZE];
    int blockCount = 0;

    for (int i = 0; i < SIZE; i++) {
        if (isValid(i) && canFormMillAt(board, i, adversarySymbol)) {
            blockPositions[blockCount++] = i;
        }
    }

    if (aiSearchDepth > 0) {
        int extraDepth = 0;
        if (oppThreatsBefore > 0) {
            extraDepth++;
        }
        if (aiThreatsBefore > 0) {
            extraDepth++;
        }
        if (twoOppBefore > 0 || twoAiBefore > 0) {
            extraDepth++;
        }
        int localDepth = clampSearchDepth(aiSearchDepth - 1 + extraDepth);
        int searchBest = -1000000;
        for (int i = 0; i < SIZE; i++) {
            if (blockCount > 0) {
                int isBlock = 0;
                for (int k = 0; k < blockCount; k++) {
                    if (blockPositions[k] == i) {
                        isBlock = 1;
                        break;
                    }
                }
                if (!isBlock) {
                    continue;
                }
            }
            if (isValid(i)) {
                board[i] = 'm';
                int score = minimaxPlacement(board, adversarySymbol, 'm', localDepth, 'm', adversarySymbol);
                board[i] = 'O';
                if (score > searchBest) {
                    searchBest = score;
                    bestPosition = i;
                }
            }
        }
        if (bestPosition != -1) {
            return bestPosition;
        }
    }
    
    for (int i = 0; i < SIZE; i++) {
        if (blockCount > 0) {
            int isBlock = 0;
            for (int k = 0; k < blockCount; k++) {
                if (blockPositions[k] == i) {
                    isBlock = 1;
                    break;
                }
            }
            if (!isBlock) {
                continue;
            }
        }
        if (isValid(i)) {
            board[i] = 'm';
            int oppThreatsAfter = countImmediateMillMoves(board, adversarySymbol);
            if (oppThreatsAfter < minOppThreatsAfter) {
                minOppThreatsAfter = oppThreatsAfter;
            }
            board[i] = 'O';
        }
    }

    for (int i = 0; i < SIZE; i++) {
        if (isValid(i)) {
            // Try this move
            board[i] = 'm';
            
            int score = 0;

            int oppThreatsAfter = countImmediateMillMoves(board, adversarySymbol);
            int oppMobilityAfter = countMobilityForSymbol(board, adversarySymbol);
            int twoOppAfter = countTwoInRowLines(board, adversarySymbol);
            int twoAiAfter = countTwoInRowLines(board, 'm');

            if (oppThreatsAfter > minOppThreatsAfter) {
                score -= 200000;
            }
            if (oppThreatsAfter > 0) {
                score -= oppThreatsAfter * 700;
            }
            if (oppThreatsBefore > oppThreatsAfter) {
                score += (oppThreatsBefore - oppThreatsAfter) * 900;
            }
            if (oppMobilityAfter < oppMobilityBefore) {
                score += (oppMobilityBefore - oppMobilityAfter) * 8;
            }
            if (twoOppAfter < twoOppBefore) {
                score += (twoOppBefore - twoOppAfter) * 180;
            }
            if (twoOppAfter > twoOppBefore) {
                score -= (twoOppAfter - twoOppBefore) * 140;
            }
            if (twoAiAfter > twoAiBefore) {
                score += (twoAiAfter - twoAiBefore) * 120;
            }
            
            // Immediate mill formation
            if (moulinOnBoard(board, i)) {
                score += 1100;
                
                // Additional points if we can capture strategically
                score += evaluateCaptureValue(i, adversarySymbol);
            }
            
            // Threat creation (potential mills)
            score += threatPlacement(board, 'm', i) * 70;
            
            // Strategic positions (intersections, center of sides)
            if (isStrategicPosition(i)) {
                score += 30;
            }
            
            // Block opponent's potential mills
            char tempBoard[SIZE];
            memcpy(tempBoard, board, SIZE);
            tempBoard[i] = adversarySymbol;
            if (moulinOnBoard(tempBoard, i)) {
                score += 240; // Blocking opponent's mill
            }
            
            // Future threat potential
            score += createsFutureThreat(i, adversarySymbol) * 90;
            
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
    int oppThreatsBefore = countImmediateMillMoves(board, adversarySymbol);
    int oppMobilityBefore = countMobilityForSymbol(board, adversarySymbol);
    int aiThreatsBefore = countImmediateMillMoves(board, 'm');
    int twoOppBefore = countTwoInRowLines(board, adversarySymbol);
    int twoAiBefore = countTwoInRowLines(board, 'm');
    int minOppThreatsAfter = 100000;
    int blockTargets[SIZE];
    int blockCount = 0;
    int hasBlockMove = 0;
    
    for (int i = 0; i < SIZE; i++) {
        if (board[i] == 'm') {
            machinePieces++;
        }
    }

    for (int i = 0; i < SIZE; i++) {
        if (canFormMillAt(board, i, adversarySymbol)) {
            blockTargets[blockCount++] = i;
        }
    }

    if (blockCount > 0) {
        for (int from = 0; from < SIZE; from++) {
            if (board[from] != 'm') {
                continue;
            }
            int moves[SIZE];
            int moveCount = getValidMovesForPiece(board, from, moves, machinePieces);
            for (int j = 0; j < moveCount; j++) {
                int to = moves[j];
                for (int k = 0; k < blockCount; k++) {
                    if (blockTargets[k] == to) {
                        hasBlockMove = 1;
                        break;
                    }
                }
                if (hasBlockMove) {
                    break;
                }
            }
            if (hasBlockMove) {
                break;
            }
        }
    }

    if (aiSearchDepth > 0) {
        int extraDepth = 0;
        if (oppThreatsBefore > 0) {
            extraDepth++;
        }
        if (aiThreatsBefore > 0) {
            extraDepth++;
        }
        if (twoOppBefore > 0 || twoAiBefore > 0) {
            extraDepth++;
        }
        int localDepth = clampSearchDepth(aiSearchDepth - 1 + extraDepth);
        int searchBest = -1000000;
        for (int from = 0; from < SIZE; from++) {
            if (board[from] != 'm') {
                continue;
            }
            int moves[SIZE];
            int moveCount = getValidMovesForPiece(board, from, moves, machinePieces);
            for (int j = 0; j < moveCount; j++) {
                int to = moves[j];
                if (hasBlockMove) {
                    int isBlock = 0;
                    for (int k = 0; k < blockCount; k++) {
                        if (blockTargets[k] == to) {
                            isBlock = 1;
                            break;
                        }
                    }
                    if (!isBlock) {
                        continue;
                    }
                }
                char originalFrom = board[from];
                char originalTo = board[to];
                board[from] = 'O';
                board[to] = 'm';

                int score = minimaxMovement(board, adversarySymbol, 'm', localDepth, 'm', adversarySymbol);

                board[from] = originalFrom;
                board[to] = originalTo;

                if (score > searchBest) {
                    searchBest = score;
                    bestMove.from = from;
                    bestMove.to = to;
                }
            }
        }
        if (bestMove.from != -1) {
            return bestMove;
        }
    }
    
    for (int from = 0; from < SIZE; from++) {
        if (board[from] == 'm') {
            int validMoves[SIZE];
            int moveCount = isValidToMove(from, validMoves, machinePieces);
            
            for (int j = 0; j < moveCount && validMoves[j] != -1; j++) {
                int to = validMoves[j];
                if (hasBlockMove) {
                    int isBlock = 0;
                    for (int k = 0; k < blockCount; k++) {
                        if (blockTargets[k] == to) {
                            isBlock = 1;
                            break;
                        }
                    }
                    if (!isBlock) {
                        continue;
                    }
                }
                
                // Try this move
                char originalFrom = board[from];
                char originalTo = board[to];
                
                board[from] = 'O';
                board[to] = 'm';
                int oppThreatsAfter = countImmediateMillMoves(board, adversarySymbol);
                if (oppThreatsAfter < minOppThreatsAfter) {
                    minOppThreatsAfter = oppThreatsAfter;
                }
                board[from] = originalFrom;
                board[to] = originalTo;
            }
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

                int oppThreatsAfter = countImmediateMillMoves(board, adversarySymbol);
                int oppMobilityAfter = countMobilityForSymbol(board, adversarySymbol);
                int twoOppAfter = countTwoInRowLines(board, adversarySymbol);
                int twoAiAfter = countTwoInRowLines(board, 'm');
                int aiMobilityAfter = countMobilityForSymbol(board, 'm');

                if (oppThreatsAfter > minOppThreatsAfter) {
                    score -= 200000;
                }

                if (oppThreatsAfter > 0) {
                    score -= oppThreatsAfter * 700;
                }
                if (oppThreatsBefore > oppThreatsAfter) {
                    score += (oppThreatsBefore - oppThreatsAfter) * 900;
                }
                if (oppMobilityAfter < oppMobilityBefore) {
                    score += (oppMobilityBefore - oppMobilityAfter) * 8;
                }
                if (twoOppAfter < twoOppBefore) {
                    score += (twoOppBefore - twoOppAfter) * 180;
                }
                if (twoOppAfter > twoOppBefore) {
                    score -= (twoOppAfter - twoOppBefore) * 140;
                }
                if (twoAiAfter > twoAiBefore) {
                    score += (twoAiAfter - twoAiBefore) * 120;
                }
                if (aiMobilityAfter < 3) {
                    score -= (3 - aiMobilityAfter) * 120;
                }
                
                // Immediate mill formation
                if (moulinOnBoard(board, to)) {
                    score += 1200;
                    
                    // Evaluate capture opportunity
                    score += evaluateCaptureValue(to, adversarySymbol);
                }
                
                // Threat creation
                score += threatMouvement(board, 'm', from, to) * 60;
                
                // Mobility improvement
                int newMobility = countMobility(to);
                int oldMobility = countMobility(from);
                score += (newMobility - oldMobility) * 5;
                
                // Strategic positioning
                if (isStrategicPosition(to)) {
                    score += 35;
                }
                
                // Double mill potential
                if (doubleMoulinPossible2(board, 'm', to)) {
                    score += 400;
                }
                
                // Block opponent
                score -= threatPlacement(board, adversarySymbol, to) * 20;
                
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
        if (pos != i && board[pos] == adversarySymbol) {
            threats++;
        }
    }
    
    // Check vertical threats based on position
    switch (i) {
        case 0: case 9: case 21:
            if ((i == 0 && (board[9] == adversarySymbol || board[21] == adversarySymbol)) ||
                (i == 9 && (board[0] == adversarySymbol || board[21] == adversarySymbol)) ||
                (i == 21 && (board[0] == adversarySymbol || board[9] == adversarySymbol))) {
                threats++;
            }
            break;
        case 1: case 4: case 7:
            if ((i == 1 && (board[4] == adversarySymbol || board[7] == adversarySymbol)) ||
                (i == 4 && (board[1] == adversarySymbol || board[7] == adversarySymbol)) ||
                (i == 7 && (board[1] == adversarySymbol || board[4] == adversarySymbol))) {
                threats++;
            }
            break;
        case 2: case 14: case 23:
            if ((i == 2 && (board[14] == adversarySymbol || board[23] == adversarySymbol)) ||
                (i == 14 && (board[2] == adversarySymbol || board[23] == adversarySymbol)) ||
                (i == 23 && (board[2] == adversarySymbol || board[14] == adversarySymbol))) {
                threats++;
            }
            break;
        case 3: case 10: case 18:
            if ((i == 3 && (board[10] == adversarySymbol || board[18] == adversarySymbol)) ||
                (i == 10 && (board[3] == adversarySymbol || board[18] == adversarySymbol)) ||
                (i == 18 && (board[3] == adversarySymbol || board[10] == adversarySymbol))) {
                threats++;
            }
            break;
        case 5: case 13: case 20:
            if ((i == 5 && (board[13] == adversarySymbol || board[20] == adversarySymbol)) ||
                (i == 13 && (board[5] == adversarySymbol || board[20] == adversarySymbol)) ||
                (i == 20 && (board[5] == adversarySymbol || board[13] == adversarySymbol))) {
                threats++;
            }
            break;
        case 6: case 11: case 15:
            if ((i == 6 && (board[11] == adversarySymbol || board[15] == adversarySymbol)) ||
                (i == 11 && (board[6] == adversarySymbol || board[15] == adversarySymbol)) ||
                (i == 15 && (board[6] == adversarySymbol || board[11] == adversarySymbol))) {
                threats++;
            }
            break;
        case 8: case 12: case 17:
            if ((i == 8 && (board[12] == adversarySymbol || board[17] == adversarySymbol)) ||
                (i == 12 && (board[8] == adversarySymbol || board[17] == adversarySymbol)) ||
                (i == 17 && (board[8] == adversarySymbol || board[12] == adversarySymbol))) {
                threats++;
            }
            break;
        case 16: case 19: case 22:
            if ((i == 16 && (board[19] == adversarySymbol || board[22] == adversarySymbol)) ||
                (i == 19 && (board[16] == adversarySymbol || board[22] == adversarySymbol)) ||
                (i == 22 && (board[16] == adversarySymbol || board[19] == adversarySymbol))) {
                threats++;
            }
            break;
    }
    
    return threats;
}

// Threat detection for movement
int threatMouvement(char *board, char adversarySymbol, int source, int destination) {
    int threats = 0;
    char originalSource = board[source];
    char originalDestination = board[destination];
    
    // Check if move creates new threats
    board[source] = 'O';
    board[destination] = adversarySymbol;
    
    // Check all possible mills from new position
    for (int i = 0; i < SIZE; i++) {
        if (board[i] == 'O') {
            board[i] = adversarySymbol;
            if (moulinOnBoard(board, i)) {
                threats++;
            }
            board[i] = 'O';
        }
    }
    
    // Reset board
    board[source] = originalSource;
    board[destination] = originalDestination;
    
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
            int score = evaluateCaptureImpact(board, i, adversarySymbol);
            score += countMobility(i) * 6;
            if (score > value) {
                value = score;
            }
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
    int millsBefore = countMillsForSymbol(board, adversarySymbol);
    int twoBefore = countTwoInRowLines(board, adversarySymbol);
    int mobilityBefore = countMobilityForSymbol(board, adversarySymbol);
    
    for (int i = 0; i < SIZE; i++) {
        if (board[i] == adversarySymbol) {
            char original = board[i];
            int score = evaluateCaptureImpact(board, i, adversarySymbol);
            board[i] = 'O';
            int millsAfter = countMillsForSymbol(board, adversarySymbol);
            int twoAfter = countTwoInRowLines(board, adversarySymbol);
            int mobilityAfter = countMobilityForSymbol(board, adversarySymbol);
            board[i] = original;

            if (millsAfter < millsBefore) {
                score += (millsBefore - millsAfter) * 80;
            }
            if (twoAfter < twoBefore) {
                score += (twoBefore - twoAfter) * 60;
            }
            if (mobilityAfter < mobilityBefore) {
                score += (mobilityBefore - mobilityAfter) * 6;
            }
            if (moulinOnBoard(board, i)) {
                score += 90;
            }
            score += countMobility(i) * 8;
            
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
    int millsBefore = countMillsForSymbol(board, adversarySymbol);
    int twoBefore = countTwoInRowLines(board, adversarySymbol);
    int mobilityBefore = countMobilityForSymbol(board, adversarySymbol);
    
    for (int i = 0; i < SIZE; i++) {
        if (board[i] == adversarySymbol) {
            char original = board[i];
            int score = evaluateCaptureImpact(board, i, adversarySymbol);
            board[i] = 'O';
            int millsAfter = countMillsForSymbol(board, adversarySymbol);
            int twoAfter = countTwoInRowLines(board, adversarySymbol);
            int mobilityAfter = countMobilityForSymbol(board, adversarySymbol);
            board[i] = original;

            if (millsAfter < millsBefore) {
                score += (millsBefore - millsAfter) * 120;
            }
            if (twoAfter < twoBefore) {
                score += (twoBefore - twoAfter) * 80;
            }
            if (mobilityAfter < mobilityBefore) {
                score += (mobilityBefore - mobilityAfter) * 10;
            }
            if (moulinOnBoard(board, i)) {
                score += 220;
            }
            score -= countMobility(i) * 4;
            
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
    if (moulinOnBoard(board, k)) {
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

static const int millLines[16][3] = {
    {0, 1, 2}, {3, 4, 5}, {6, 7, 8},
    {9, 10, 11}, {12, 13, 14}, {15, 16, 17},
    {18, 19, 20}, {21, 22, 23},
    {0, 9, 21}, {1, 4, 7}, {2, 14, 23}, {3, 10, 18},
    {5, 13, 20}, {6, 11, 15}, {8, 12, 17}, {16, 19, 22}
};

static int countMillsForSymbol(const char *b, char symbol) {
    int count = 0;
    for (int i = 0; i < 16; i++) {
        if (b[millLines[i][0]] == symbol &&
            b[millLines[i][1]] == symbol &&
            b[millLines[i][2]] == symbol) {
            count++;
        }
    }
    return count;
}

static int countTwoInRowLines(const char *b, char symbol) {
    int count = 0;
    for (int i = 0; i < 16; i++) {
        int a = millLines[i][0];
        int c = millLines[i][1];
        int d = millLines[i][2];
        int symbols = 0;
        int empties = 0;

        if (b[a] == symbol) symbols++; else if (b[a] == 'O') empties++;
        if (b[c] == symbol) symbols++; else if (b[c] == 'O') empties++;
        if (b[d] == symbol) symbols++; else if (b[d] == 'O') empties++;

        if (symbols == 2 && empties == 1) {
            count++;
        }
    }
    return count;
}

static int clampSearchDepth(int depth) {
    if (depth < 0) {
        return 0;
    }
    if (depth > 5) {
        return 5;
    }
    return depth;
}