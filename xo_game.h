#pragma once

#include <windows.h>
#include <string>
#include <array>
#include <vector>
#include <memory>
#include <functional>
#include "ai_player.h"

class XOGame {
public:
    XOGame(HINSTANCE hInstance);
    ~XOGame();
    
    int Run(int nCmdShow);
    
private:
    // Game states
    enum class GameScreen { Welcome, Game, GameOver };
    enum class CellState { Empty, X, O };
    enum class GameState { Playing, XWon, OWon, Draw };
    enum class PlayerType { Human, AI };
    enum class AIDifficulty { Easy, Normal, Hard };
    
    // Window procedure
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    
    // Drawing functions
    void OnPaint(HWND hwnd);
    void DrawWelcomeScreen(HDC hdc);
    void DrawGameScreen(HDC hdc);
    void DrawGameOverScreen(HDC hdc);
    void DrawButton(HDC hdc, const std::wstring& text, RECT rect, bool isHovered, bool isActive = false);
    void DrawBoard(HDC hdc);
    void DrawCell(HDC hdc, int row, int col);
    
    // Input handling
    void OnMouseMove(int x, int y);
    void OnMouseClick(int x, int y);
    
    // Game logic
    void StartGame(PlayerType xPlayerType, PlayerType oPlayerType);
    void ResetGame();
    void CheckGameStatus();
    void SwitchPlayer();
    void UpdateStatusText();
    void MakeAIMove();
    
    // UI constants
    static constexpr int GRID_SIZE = 3;
    static constexpr int CELL_SIZE = 120;    // Increased from 100
    static constexpr int WINDOW_WIDTH = 500;  // Fixed window width
    static constexpr int WINDOW_HEIGHT = 600; // Fixed window height
    static constexpr int BUTTON_HEIGHT = 45;  // Increased from 40
    static constexpr int BUTTON_WIDTH = 180;  // Increased from 150
    static constexpr int BUTTON_PADDING = 15; // Increased from 10
    
    // UI Resources
    HINSTANCE m_hInstance;
    HWND m_hwnd;
    HFONT m_titleFont;
    HFONT m_buttonFont;
    HFONT m_gameFont;
    HFONT m_statusFont;
    HBRUSH m_backgroundBrush;
    HBRUSH m_cellBrush;
    HBRUSH m_hoverBrush;
    HBRUSH m_buttonBrush;
    HBRUSH m_buttonHoverBrush;
    HBRUSH m_buttonActiveBrush;
    HBRUSH m_buttonFrameBrush;
    HPEN m_gridPen;
    
    // UI State - keep in the same order as initialized in constructor
    GameScreen m_currentScreen;
    int m_hoveredButton;
    int m_hoverRow;
    int m_hoverCol;
    std::vector<std::pair<RECT, std::function<void()>>> m_buttons;
    
    // Game State
    GameState m_gameState;
    CellState m_currentPlayer;
    std::wstring m_statusText;
    PlayerType m_xPlayerType;
    PlayerType m_oPlayerType;
    AIDifficulty m_aiDifficulty;
    std::array<std::array<CellState, GRID_SIZE>, GRID_SIZE> m_board;
    
    // AI
    std::unique_ptr<AIPlayer> m_aiPlayer;
}; 