#pragma once

#include <array>
#include <utility>
#include <random>

// Forward declaration
class XOGame;

class AIPlayer {
public:
    AIPlayer();

    enum class CellState { Empty, X, O };
    enum class Difficulty { Easy, Normal, Hard };
    
    // Calculate the best move for the AI based on difficulty
    std::pair<int, int> GetBestMove(const std::array<std::array<CellState, 3>, 3>& board, CellState aiPlayer, Difficulty difficulty = Difficulty::Hard);
    
    // Overload for XOGame's CellState enum
    template <typename T>
    std::pair<int, int> GetBestMove(const std::array<std::array<T, 3>, 3>& board, T aiPlayer, Difficulty difficulty = Difficulty::Hard) {
        // Convert board from XOGame::CellState to AIPlayer::CellState
        std::array<std::array<CellState, 3>, 3> convertedBoard;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == T::Empty) 
                    convertedBoard[i][j] = CellState::Empty;
                else if (board[i][j] == T::X) 
                    convertedBoard[i][j] = CellState::X;
                else 
                    convertedBoard[i][j] = CellState::O;
            }
        }
        
        // Convert aiPlayer from XOGame::CellState to AIPlayer::CellState
        CellState convertedAiPlayer = (aiPlayer == T::X) ? CellState::X : CellState::O;
        
        // Call the original GetBestMove with converted parameters
        return GetBestMove(convertedBoard, convertedAiPlayer, difficulty);
    }

private:
    // Minimax algorithm with alpha-beta pruning
    int Minimax(std::array<std::array<CellState, 3>, 3> board, int depth, bool isMaximizing, 
                CellState aiPlayer, CellState humanPlayer, int alpha, int beta);
    
    // Easy difficulty - make random valid moves
    std::pair<int, int> GetRandomMove(const std::array<std::array<CellState, 3>, 3>& board);

    // Normal difficulty - sometimes make good moves, sometimes random
    std::pair<int, int> GetIntermediateMove(const std::array<std::array<CellState, 3>, 3>& board, CellState aiPlayer);
    
    // Check if someone has won
    bool CheckWin(const std::array<std::array<CellState, 3>, 3>& board, CellState player);
    
    // Check if the board is full
    bool IsBoardFull(const std::array<std::array<CellState, 3>, 3>& board);
    
    // Evaluate the board for minimax (returns +10 for AI win, -10 for player win, 0 for draw or ongoing)
    int EvaluateBoard(const std::array<std::array<CellState, 3>, 3>& board, CellState aiPlayer, CellState humanPlayer);
    
    std::random_device m_rd;
    std::mt19937 m_rng;
}; 