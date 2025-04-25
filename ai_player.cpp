#include "ai_player.h"
#include <algorithm>

AIPlayer::AIPlayer() : m_rng(m_rd()) {
}

std::pair<int, int> AIPlayer::GetBestMove(const std::array<std::array<CellState, 3>, 3>& board, CellState aiPlayer, Difficulty difficulty) {
    // Choose the move based on difficulty level
    switch (difficulty) {
        case Difficulty::Easy:
            return GetRandomMove(board);
            
        case Difficulty::Normal:
            return GetIntermediateMove(board, aiPlayer);
            
        case Difficulty::Hard:
        default:
            // Hard difficulty - use full minimax algorithm
            CellState humanPlayer = (aiPlayer == CellState::X) ? CellState::O : CellState::X;
            
            int bestScore = -1000;
            std::pair<int, int> bestMove = {-1, -1};
            
            // Check for available moves and evaluate each one
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (board[i][j] == CellState::Empty) {
                        // Try this move
                        auto boardCopy = board;
                        boardCopy[i][j] = aiPlayer;
                        
                        // Calculate score for this move using minimax
                        int score = Minimax(boardCopy, 0, false, aiPlayer, humanPlayer, -1000, 1000);
                        
                        // If this move has a better score than our best move so far, update bestMove
                        if (score > bestScore) {
                            bestScore = score;
                            bestMove = {i, j};
                        }
                    }
                }
            }
            
            return bestMove;
    }
}

std::pair<int, int> AIPlayer::GetRandomMove(const std::array<std::array<CellState, 3>, 3>& board) {
    // Count empty cells
    std::vector<std::pair<int, int>> emptyCells;
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == CellState::Empty) {
                emptyCells.push_back({i, j});
            }
        }
    }
    
    // If no empty cells, return invalid move
    if (emptyCells.empty()) {
        return {-1, -1};
    }
    
    // Pick a random empty cell
    std::uniform_int_distribution<int> dist(0, emptyCells.size() - 1);
    int randomIndex = dist(m_rng);
    
    return emptyCells[randomIndex];
}

std::pair<int, int> AIPlayer::GetIntermediateMove(const std::array<std::array<CellState, 3>, 3>& board, CellState aiPlayer) {
    // 60% of the time, make an optimal move
    // 40% of the time, make a random move
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double randomVal = dist(m_rng);
    
    if (randomVal < 0.6) {
        // Make an optimal move
        CellState humanPlayer = (aiPlayer == CellState::X) ? CellState::O : CellState::X;
        
        int bestScore = -1000;
        std::pair<int, int> bestMove = {-1, -1};
        
        // Try each possible move
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == CellState::Empty) {
                    // Try this move
                    auto boardCopy = board;
                    boardCopy[i][j] = aiPlayer;
                    
                    // Calculate score for this move using minimax
                    int score = Minimax(boardCopy, 0, false, aiPlayer, humanPlayer, -1000, 1000);
                    
                    // If this move has a better score than our best move so far, update bestMove
                    if (score > bestScore) {
                        bestScore = score;
                        bestMove = {i, j};
                    }
                }
            }
        }
        
        return bestMove;
    } 
    else {
        // Make a random move
        return GetRandomMove(board);
    }
}

int AIPlayer::Minimax(std::array<std::array<CellState, 3>, 3> board, int depth, bool isMaximizing, 
                      CellState aiPlayer, CellState humanPlayer, int alpha, int beta) {
    // Check terminal states
    int score = EvaluateBoard(board, aiPlayer, humanPlayer);
    
    // If we have a winner or board is full, return the score
    if (score == 10 || score == -10 || IsBoardFull(board)) {
        return score;
    }
    
    // AI's turn (maximizing player)
    if (isMaximizing) {
        int bestScore = -1000;
        
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == CellState::Empty) {
                    board[i][j] = aiPlayer;
                    bestScore = std::max(bestScore, Minimax(board, depth + 1, false, aiPlayer, humanPlayer, alpha, beta));
                    board[i][j] = CellState::Empty;
                    
                    // Alpha-beta pruning
                    alpha = std::max(alpha, bestScore);
                    if (beta <= alpha) {
                        break;
                    }
                }
            }
        }
        
        return bestScore;
    }
    // Human's turn (minimizing player)
    else {
        int bestScore = 1000;
        
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == CellState::Empty) {
                    board[i][j] = humanPlayer;
                    bestScore = std::min(bestScore, Minimax(board, depth + 1, true, aiPlayer, humanPlayer, alpha, beta));
                    board[i][j] = CellState::Empty;
                    
                    // Alpha-beta pruning
                    beta = std::min(beta, bestScore);
                    if (beta <= alpha) {
                        break;
                    }
                }
            }
        }
        
        return bestScore;
    }
}

bool AIPlayer::CheckWin(const std::array<std::array<CellState, 3>, 3>& board, CellState player) {
    // Check rows
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == player && board[i][1] == player && board[i][2] == player) {
            return true;
        }
    }
    
    // Check columns
    for (int i = 0; i < 3; i++) {
        if (board[0][i] == player && board[1][i] == player && board[2][i] == player) {
            return true;
        }
    }
    
    // Check diagonals
    if (board[0][0] == player && board[1][1] == player && board[2][2] == player) {
        return true;
    }
    
    if (board[0][2] == player && board[1][1] == player && board[2][0] == player) {
        return true;
    }
    
    return false;
}

bool AIPlayer::IsBoardFull(const std::array<std::array<CellState, 3>, 3>& board) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == CellState::Empty) {
                return false;
            }
        }
    }
    return true;
}

int AIPlayer::EvaluateBoard(const std::array<std::array<CellState, 3>, 3>& board, CellState aiPlayer, CellState humanPlayer) {
    if (CheckWin(board, aiPlayer)) {
        return 10;
    }
    
    if (CheckWin(board, humanPlayer)) {
        return -10;
    }
    
    return 0; // No winner, game ongoing or draw
} 