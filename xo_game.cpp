#include "xo_game.h"
#include <windowsx.h>
#include <algorithm>

// Define resources manually for g++ compatibility
#ifndef IDI_APPICON
#define IDI_APPICON 101
#endif

// Define for Unicode handling with g++
#ifdef __GNUC__
    #define UNICODE_AWARE(funcA, funcW) funcA
#else
    #define UNICODE_AWARE(funcA, funcW) funcW
#endif

const char CLASS_NAME_A[] = "XOGameWindow";
const wchar_t CLASS_NAME_W[] = L"XOGameWindow";

// Color definitions for modern UI
#define UI_BACKGROUND RGB(245, 245, 245)  // Lighter background
#define COLOR_CELL       RGB(255, 255, 255)  // White cells
#define COLOR_HOVER      RGB(235, 245, 255)  // Light blue hover
#define COLOR_GRID       RGB(60, 60, 60)     // Darker gray grid
#define COLOR_X          RGB(41, 128, 185)   // Bright blue for X
#define COLOR_O          RGB(231, 76, 60)    // Bright red for O
#define COLOR_BUTTON     RGB(52, 152, 219)   // Bright blue
#define COLOR_BUTTON_HOVER RGB(41, 128, 185)  // Darker blue for hover
#define COLOR_BUTTON_ACTIVE RGB(25, 99, 145)  // Even darker blue for active/pressed

XOGame::XOGame(HINSTANCE hInstance) 
    : m_hInstance(hInstance), 
      m_hwnd(NULL), 
      m_currentScreen(GameScreen::Welcome),
      m_hoveredButton(-1),
      m_hoverRow(-1),
      m_hoverCol(-1),
      m_gameState(GameState::Playing),
      m_currentPlayer(CellState::X),
      m_xPlayerType(PlayerType::Human),
      m_oPlayerType(PlayerType::AI),
      m_aiDifficulty(AIDifficulty::Normal) {
      
    // Create AI player
    m_aiPlayer = std::make_unique<AIPlayer>();
    
    // Initialize the board
    ResetGame();
    
    // Register the window class
    WNDCLASSEX wcex = {0};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = XOGame::WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_HAND);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    
    #ifdef __GNUC__
        wcex.lpszClassName = CLASS_NAME_A;
    #else
        wcex.lpszClassName = CLASS_NAME_W;
    #endif
    
    // Load application icon from app.ico
    HICON hIcon = (HICON)LoadImage(NULL, "app.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
    if (hIcon) {
        wcex.hIcon = hIcon;
        wcex.hIconSm = hIcon;
    } else {
        // Fallback to default icon if app.ico cannot be loaded
        wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    }
    
    RegisterClassEx(&wcex);
    
    // Create the window
    #ifdef __GNUC__
        m_hwnd = CreateWindowExA(
            0,                             // Optional window styles
            CLASS_NAME_A,                  // Window class
            "XO Game",              // Window text
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,  // Window style - no resizing
            (GetSystemMetrics(SM_CXSCREEN) - WINDOW_WIDTH) / 2,  // Center X on screen
            (GetSystemMetrics(SM_CYSCREEN) - WINDOW_HEIGHT) / 2, // Center Y on screen
            WINDOW_WIDTH,                  // Width (exact window width)
            WINDOW_HEIGHT,                 // Height (exact window height)
            NULL,                          // Parent window    
            NULL,                          // Menu
            hInstance,                     // Instance handle
            this                           // Additional application data
        );
    #else
        m_hwnd = CreateWindowExW(
            0,                             // Optional window styles
            CLASS_NAME_W,                  // Window class
            L"XO Game",             // Window text
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,  // Window style - no resizing
            (GetSystemMetrics(SM_CXSCREEN) - WINDOW_WIDTH) / 2,  // Center X on screen
            (GetSystemMetrics(SM_CYSCREEN) - WINDOW_HEIGHT) / 2, // Center Y on screen
            WINDOW_WIDTH,                  // Width (exact window width)
            WINDOW_HEIGHT,                 // Height (exact window height)
            NULL,                          // Parent window    
            NULL,                          // Menu
            hInstance,                     // Instance handle
            this                           // Additional application data
        );
    #endif
    
    // Adjust window size to account for borders to ensure client area is exactly WINDOW_WIDTH x WINDOW_HEIGHT
    if (m_hwnd) {
        RECT clientRect, windowRect;
        GetClientRect(m_hwnd, &clientRect);
        GetWindowRect(m_hwnd, &windowRect);
        
        int borderWidth = (windowRect.right - windowRect.left) - clientRect.right;
        int borderHeight = (windowRect.bottom - windowRect.top) - clientRect.bottom;
        
        SetWindowPos(m_hwnd, NULL, 0, 0, 
                    WINDOW_WIDTH + borderWidth, 
                    WINDOW_HEIGHT + borderHeight, 
                    SWP_NOMOVE | SWP_NOZORDER);
    }
    
    // Create fonts for modern UI
    #ifdef __GNUC__
        m_titleFont = CreateFontA(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 
                          DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, 
                          CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
                          
        m_buttonFont = CreateFontA(20, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, 
                         DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, 
                         CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
                         
        m_gameFont = CreateFontA(60, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 
                       DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, 
                       CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
                         
        m_statusFont = CreateFontA(24, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, 
                          DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, 
                          CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    #else
        m_titleFont = CreateFontW(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 
                         DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, 
                         CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
                         
        m_buttonFont = CreateFontW(20, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, 
                          DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, 
                          CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
                          
        m_gameFont = CreateFontW(60, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 
                        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, 
                        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
                         
        m_statusFont = CreateFontW(24, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, 
                         DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, 
                         CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    #endif
                              
    // Create brushes for modern UI
    m_backgroundBrush = CreateSolidBrush(UI_BACKGROUND);
    m_cellBrush = CreateSolidBrush(COLOR_CELL);
    m_hoverBrush = CreateSolidBrush(COLOR_HOVER);
    m_buttonBrush = CreateSolidBrush(COLOR_BUTTON);
    m_buttonHoverBrush = CreateSolidBrush(COLOR_BUTTON_HOVER);
    m_buttonActiveBrush = CreateSolidBrush(COLOR_BUTTON_ACTIVE);
    m_buttonFrameBrush = CreateSolidBrush(RGB(20, 20, 20)); // Dark frame for buttons
    m_gridPen = CreatePen(PS_SOLID, 2, COLOR_GRID);
    
    UpdateStatusText();
}

XOGame::~XOGame() {
    // Clean up GDI resources
    DeleteObject(m_titleFont);
    DeleteObject(m_buttonFont);
    DeleteObject(m_gameFont);
    DeleteObject(m_statusFont);
    DeleteObject(m_backgroundBrush);
    DeleteObject(m_cellBrush);
    DeleteObject(m_hoverBrush);
    DeleteObject(m_buttonBrush);
    DeleteObject(m_buttonHoverBrush);
    DeleteObject(m_buttonActiveBrush);
    DeleteObject(m_buttonFrameBrush);
    DeleteObject(m_gridPen);
}

int XOGame::Run(int nCmdShow) {
    // Show the window
    ShowWindow(m_hwnd, nCmdShow);
    UpdateWindow(m_hwnd);
    
    // Start message loop
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (int)msg.wParam;
}

LRESULT CALLBACK XOGame::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    XOGame* game = nullptr;
    
    // Get the game instance on WM_CREATE
    if (message == WM_CREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        game = reinterpret_cast<XOGame*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(game));
    } else {
        game = reinterpret_cast<XOGame*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }
    
    // If we have a game instance, let it handle the message
    if (game) {
        return game->HandleMessage(hwnd, message, wParam, lParam);
    } else {
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
}

LRESULT XOGame::HandleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_PAINT:
            OnPaint(hwnd);
            return 0;
            
        case WM_MOUSEMOVE: {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);
            OnMouseMove(xPos, yPos);
            return 0;
        }
            
        case WM_LBUTTONDOWN: {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);
            OnMouseClick(xPos, yPos);
            return 0;
        }
            
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                // Reset game on ESC key
                if (m_currentScreen == GameScreen::Game) {
                    m_currentScreen = GameScreen::Welcome;
                    InvalidateRect(hwnd, NULL, FALSE);
                } else if (m_currentScreen == GameScreen::Welcome) {
                    // Exit on welcome screen
                    PostQuitMessage(0);
                }
            }
            return 0;
            
        case WM_TIMER:
            if (wParam == 1) {
                // Timer for AI move
                KillTimer(hwnd, 1);
                if (m_currentScreen == GameScreen::Game && m_gameState == GameState::Playing) {
                    MakeAIMove();
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            return 0;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
}

void XOGame::OnPaint(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    
    // Get the actual client area dimensions
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    int clientWidth = clientRect.right - clientRect.left;
    int clientHeight = clientRect.bottom - clientRect.top;
    
    // Create compatible DC for double buffering
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, clientWidth, clientHeight);
    SelectObject(memDC, memBitmap);
    
    // Fill background
    FillRect(memDC, &clientRect, m_backgroundBrush);
    
    // Draw the current screen
    switch (m_currentScreen) {
        case GameScreen::Welcome:
            DrawWelcomeScreen(memDC);
            break;
        case GameScreen::Game:
            DrawGameScreen(memDC);
            break;
        case GameScreen::GameOver:
            DrawGameOverScreen(memDC);
            break;
    }
    
    // Copy from memory DC to screen
    BitBlt(hdc, 0, 0, clientWidth, clientHeight, memDC, 0, 0, SRCCOPY);
    
    // Clean up
    DeleteObject(memBitmap);
    DeleteDC(memDC);
    
    EndPaint(hwnd, &ps);
}

void XOGame::DrawWelcomeScreen(HDC hdc) {
    // Clear buttons
    m_buttons.clear();
    
    // Calculate left margin for buttons and labels aligned with the board
    int leftMargin = (WINDOW_WIDTH - 400) / 2; // Center the content in a 400px wide area
    
    // Set up for text output
    SetBkMode(hdc, TRANSPARENT);
    SelectObject(hdc, m_titleFont);
    
    // Draw title shadow first (slight offset)
    RECT shadowRect = {2, 62, WINDOW_WIDTH, 112};
    SetTextColor(hdc, RGB(100, 100, 100));
    #ifdef __GNUC__
        DrawTextA(hdc, "XO Game", -1, &shadowRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    #else
        DrawTextW(hdc, L"XO Game", -1, &shadowRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    #endif
    
    // Draw title - positioned higher
    SetTextColor(hdc, RGB(0, 0, 0));
    RECT titleRect = {0, 60, WINDOW_WIDTH, 110};
    #ifdef __GNUC__
        DrawTextA(hdc, "XO Game", -1, &titleRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    #else
        DrawTextW(hdc, L"XO Game", -1, &titleRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    #endif
    
    // Draw player selection options - moved down for better spacing
    SelectObject(hdc, m_statusFont);
    SetTextColor(hdc, RGB(0, 0, 0));
    
    // Player X section positioned lower
    RECT playerXLabel = {leftMargin, 140, WINDOW_WIDTH - leftMargin, 170};
    SetTextColor(hdc, RGB(0, 0, 0));
    #ifdef __GNUC__
        DrawTextA(hdc, "Player X:", -1, &playerXLabel, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    #else
        DrawTextW(hdc, L"Player X:", -1, &playerXLabel, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    #endif
    
    // Player X options (Human or AI)
    RECT humanXButtonRect = {
        leftMargin + BUTTON_PADDING, 
        170, 
        leftMargin + BUTTON_PADDING + BUTTON_WIDTH, 
        170 + BUTTON_HEIGHT
    };
    
    DrawButton(hdc, L"Human", humanXButtonRect, m_hoveredButton == (int)m_buttons.size(), m_xPlayerType == PlayerType::Human);
    m_buttons.push_back({humanXButtonRect, [this]() {
        m_xPlayerType = PlayerType::Human;
        InvalidateRect(m_hwnd, NULL, FALSE);
    }});
    
    RECT aiXButtonRect = {
        leftMargin + BUTTON_PADDING + BUTTON_WIDTH + BUTTON_PADDING, 
        170, 
        leftMargin + BUTTON_PADDING + BUTTON_WIDTH * 2 + BUTTON_PADDING, 
        170 + BUTTON_HEIGHT
    };
    
    DrawButton(hdc, L"AI", aiXButtonRect, m_hoveredButton == (int)m_buttons.size(), m_xPlayerType == PlayerType::AI);
    m_buttons.push_back({aiXButtonRect, [this]() {
        m_xPlayerType = PlayerType::AI;
        InvalidateRect(m_hwnd, NULL, FALSE);
    }});
    
    // Player O options - moved down more
    SetTextColor(hdc, RGB(0, 0, 0));
    RECT playerOLabel = {leftMargin, 230, WINDOW_WIDTH - leftMargin, 260};
    #ifdef __GNUC__
        DrawTextA(hdc, "Player O:", -1, &playerOLabel, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    #else
        DrawTextW(hdc, L"Player O:", -1, &playerOLabel, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    #endif
    
    RECT humanOButtonRect = {
        leftMargin + BUTTON_PADDING, 
        260, 
        leftMargin + BUTTON_PADDING + BUTTON_WIDTH, 
        260 + BUTTON_HEIGHT
    };
    
    DrawButton(hdc, L"Human", humanOButtonRect, m_hoveredButton == (int)m_buttons.size(), m_oPlayerType == PlayerType::Human);
    m_buttons.push_back({humanOButtonRect, [this]() {
        m_oPlayerType = PlayerType::Human;
        InvalidateRect(m_hwnd, NULL, FALSE);
    }});
    
    RECT aiOButtonRect = {
        leftMargin + BUTTON_PADDING + BUTTON_WIDTH + BUTTON_PADDING, 
        260, 
        leftMargin + BUTTON_PADDING + BUTTON_WIDTH * 2 + BUTTON_PADDING, 
        260 + BUTTON_HEIGHT
    };
    
    DrawButton(hdc, L"AI", aiOButtonRect, m_hoveredButton == (int)m_buttons.size(), m_oPlayerType == PlayerType::AI);
    m_buttons.push_back({aiOButtonRect, [this]() {
        m_oPlayerType = PlayerType::AI;
        InvalidateRect(m_hwnd, NULL, FALSE);
    }});
    
    // Show AI difficulty options if either player is AI
    if (m_xPlayerType == PlayerType::AI || m_oPlayerType == PlayerType::AI) {
        RECT difficultyLabel = {leftMargin, 320, WINDOW_WIDTH - leftMargin, 350};
        SetTextColor(hdc, RGB(0, 0, 0));
        #ifdef __GNUC__
            DrawTextA(hdc, "AI Difficulty:", -1, &difficultyLabel, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        #else
            DrawTextW(hdc, L"AI Difficulty:", -1, &difficultyLabel, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        #endif
        
        // Draw three difficulty buttons side by side
        int buttonWidth = BUTTON_WIDTH / 2;
        int totalWidth = buttonWidth * 3 + BUTTON_PADDING * 2;
        int startX = WINDOW_WIDTH / 2 - totalWidth / 2;
        
        // Easy button
        RECT easyButtonRect = {
            startX, 
            350, 
            startX + buttonWidth, 
            350 + BUTTON_HEIGHT
        };
        
        DrawButton(hdc, L"Easy", easyButtonRect, m_hoveredButton == (int)m_buttons.size(), m_aiDifficulty == AIDifficulty::Easy);
        m_buttons.push_back({easyButtonRect, [this]() {
            m_aiDifficulty = AIDifficulty::Easy;
            InvalidateRect(m_hwnd, NULL, FALSE);
        }});
        
        // Normal button
        RECT normalButtonRect = {
            startX + buttonWidth + BUTTON_PADDING, 
            350, 
            startX + buttonWidth * 2 + BUTTON_PADDING, 
            350 + BUTTON_HEIGHT
        };
        
        DrawButton(hdc, L"Normal", normalButtonRect, m_hoveredButton == (int)m_buttons.size(), m_aiDifficulty == AIDifficulty::Normal);
        m_buttons.push_back({normalButtonRect, [this]() {
            m_aiDifficulty = AIDifficulty::Normal;
            InvalidateRect(m_hwnd, NULL, FALSE);
        }});
        
        // Hard button
        RECT hardButtonRect = {
            startX + buttonWidth * 2 + BUTTON_PADDING * 2, 
            350, 
            startX + buttonWidth * 3 + BUTTON_PADDING * 2, 
            350 + BUTTON_HEIGHT
        };
        
        DrawButton(hdc, L"Hard", hardButtonRect, m_hoveredButton == (int)m_buttons.size(), m_aiDifficulty == AIDifficulty::Hard);
        m_buttons.push_back({hardButtonRect, [this]() {
            m_aiDifficulty = AIDifficulty::Hard;
            InvalidateRect(m_hwnd, NULL, FALSE);
        }});
    }
    
    // Start game button - centered and positioned lower
    RECT startButtonRect = {
        WINDOW_WIDTH / 2 - BUTTON_WIDTH / 2, 
        420, 
        WINDOW_WIDTH / 2 + BUTTON_WIDTH / 2, 
        420 + BUTTON_HEIGHT
    };
    
    DrawButton(hdc, L"Start Game", startButtonRect, m_hoveredButton == (int)m_buttons.size());
    m_buttons.push_back({startButtonRect, [this]() {
        StartGame(m_xPlayerType, m_oPlayerType);
    }});
    
    // Instructions - moved to bottom
    SelectObject(hdc, m_statusFont);
    SetTextColor(hdc, RGB(0, 0, 0));
    RECT instructionsRect = {0, 500, WINDOW_WIDTH, 530};
    #ifdef __GNUC__
        DrawTextA(hdc, "Press ESC to exit", -1, &instructionsRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    #else
        DrawTextW(hdc, L"Press ESC to exit", -1, &instructionsRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    #endif
}

void XOGame::DrawGameScreen(HDC hdc) {
    // Clear buttons
    m_buttons.clear();
    
    // Draw the game board
    DrawBoard(hdc);
    
    // Get the board position from DrawBoard
    int offsetY = (WINDOW_HEIGHT - (GRID_SIZE * CELL_SIZE)) / 2 - 60;
    
    // Draw the status text in a better position
    RECT statusRect = {
        0, 
        offsetY + GRID_SIZE * CELL_SIZE + 20, 
        WINDOW_WIDTH, 
        offsetY + GRID_SIZE * CELL_SIZE + 50
    };
    
    SelectObject(hdc, m_statusFont);
    SetTextColor(hdc, RGB(0, 0, 0));
    SetBkMode(hdc, TRANSPARENT);
    
    #ifdef __GNUC__
        char ansiText[256];
        wcstombs(ansiText, m_statusText.c_str(), sizeof(ansiText));
        DrawTextA(hdc, ansiText, -1, &statusRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    #else
        DrawTextW(hdc, m_statusText.c_str(), -1, &statusRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    #endif
    
    // Add menu button - centered horizontally
    RECT menuButtonRect = {
        WINDOW_WIDTH / 2 - BUTTON_WIDTH / 2,
        offsetY + GRID_SIZE * CELL_SIZE + 70,
        WINDOW_WIDTH / 2 + BUTTON_WIDTH / 2,
        offsetY + GRID_SIZE * CELL_SIZE + 70 + BUTTON_HEIGHT
    };
    
    DrawButton(hdc, L"Menu", menuButtonRect, m_hoveredButton == (int)m_buttons.size());
    m_buttons.push_back({menuButtonRect, [this]() {
        m_currentScreen = GameScreen::Welcome;
        InvalidateRect(m_hwnd, NULL, FALSE);
    }});
}

void XOGame::DrawGameOverScreen(HDC hdc) {
    // Clear buttons
    m_buttons.clear();
    
    // Draw the game board (shows final state)
    DrawBoard(hdc);
    
    // Create semi-transparent overlay
    RECT overlayRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    
    // Create a semi-transparent brush
    COLORREF overlayColor = RGB(240, 240, 240); // Light gray
    HBRUSH overlayBrush = CreateSolidBrush(overlayColor);
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 180, 0}; // 70% opacity
    
    // Create a memory DC for the overlay
    HDC overlayDC = CreateCompatibleDC(hdc);
    HBITMAP overlayBitmap = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
    SelectObject(overlayDC, overlayBitmap);
    
    // Fill with the overlay color
    FillRect(overlayDC, &overlayRect, overlayBrush);
    
    // Copy with alpha blending
    AlphaBlend(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, overlayDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, blend);
    
    // Clean up overlay resources
    DeleteObject(overlayBitmap);
    DeleteDC(overlayDC);
    DeleteObject(overlayBrush);
    
    // Get board positioning from DrawBoard
    int offsetY = (WINDOW_HEIGHT - (GRID_SIZE * CELL_SIZE)) / 2 - 60;
    
    // Draw game over message
    SelectObject(hdc, m_titleFont);
    SetTextColor(hdc, RGB(0, 0, 0));
    SetBkMode(hdc, TRANSPARENT);
    
    // Position title at the top-center of the board area
    RECT titleRect = {0, offsetY - 10, WINDOW_WIDTH, offsetY + 60};
    
    std::wstring gameOverText;
    if (m_gameState == GameState::XWon) {
        gameOverText = L"Player X Wins!";
    } else if (m_gameState == GameState::OWon) {
        gameOverText = L"Player O Wins!";
    } else {
        gameOverText = L"Game Draw!";
    }
    
    #ifdef __GNUC__
        char ansiText[256];
        wcstombs(ansiText, gameOverText.c_str(), sizeof(ansiText));
        DrawTextA(hdc, ansiText, -1, &titleRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    #else
        DrawTextW(hdc, gameOverText.c_str(), -1, &titleRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    #endif
    
    // Position buttons at the bottom of the game area
    int buttonY = offsetY + GRID_SIZE * CELL_SIZE + 20;
    
    // Add play again button - on the left side
    RECT playAgainButtonRect = {
        WINDOW_WIDTH / 2 - BUTTON_WIDTH - 10,
        buttonY,
        WINDOW_WIDTH / 2 - 10,
        buttonY + BUTTON_HEIGHT
    };
    
    DrawButton(hdc, L"Play Again", playAgainButtonRect, m_hoveredButton == (int)m_buttons.size());
    m_buttons.push_back({playAgainButtonRect, [this]() {
        StartGame(m_xPlayerType, m_oPlayerType);
    }});
    
    // Add menu button - on the right side
    RECT menuButtonRect = {
        WINDOW_WIDTH / 2 + 10,
        buttonY,
        WINDOW_WIDTH / 2 + BUTTON_WIDTH + 10,
        buttonY + BUTTON_HEIGHT
    };
    
    DrawButton(hdc, L"Main Menu", menuButtonRect, m_hoveredButton == (int)m_buttons.size());
    m_buttons.push_back({menuButtonRect, [this]() {
        m_currentScreen = GameScreen::Welcome;
        InvalidateRect(m_hwnd, NULL, FALSE);
    }});
}

void XOGame::DrawButton(HDC hdc, const std::wstring& text, RECT rect, bool isHovered, bool isActive) {
    // Select the appropriate brush based on state
    HBRUSH buttonBrush;
    if (isActive) {
        buttonBrush = m_buttonActiveBrush;
    } else if (isHovered) {
        buttonBrush = m_buttonHoverBrush;
    } else {
        buttonBrush = m_buttonBrush;
    }
    
    // Draw button background
    FillRect(hdc, &rect, buttonBrush);
    
    // Draw button frame
    FrameRect(hdc, &rect, m_buttonFrameBrush);
    
    // Draw button text - always white for good contrast
    SelectObject(hdc, m_buttonFont);
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);
    
    #ifdef __GNUC__
        char ansiText[256];
        wcstombs(ansiText, text.c_str(), sizeof(ansiText));
        DrawTextA(hdc, ansiText, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    #else
        DrawTextW(hdc, text.c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    #endif
}

void XOGame::DrawBoard(HDC hdc) {
    // Draw grid
    SelectObject(hdc, m_gridPen);
    
    // Center the board in the window - adjust to place it a little higher
    int offsetX = (WINDOW_WIDTH - (GRID_SIZE * CELL_SIZE)) / 2;
    int offsetY = (WINDOW_HEIGHT - (GRID_SIZE * CELL_SIZE)) / 2 - 60; // Move it a bit higher
    
    // Draw a background for the board
    RECT boardRect = {
        offsetX - 5,
        offsetY - 5,
        offsetX + GRID_SIZE * CELL_SIZE + 5,
        offsetY + GRID_SIZE * CELL_SIZE + 5
    };
    
    // Fill with white background
    HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(hdc, &boardRect, whiteBrush);
    DeleteObject(whiteBrush);
    
    // Draw board frame
    HPEN framePen = CreatePen(PS_SOLID, 3, RGB(50, 50, 50));
    SelectObject(hdc, framePen);
    
    // Draw frame around the board
    Rectangle(hdc, boardRect.left, boardRect.top, boardRect.right, boardRect.bottom);
    
    // Switch back to grid pen for interior lines
    SelectObject(hdc, m_gridPen);
    
    // Draw vertical lines
    for (int i = 1; i < GRID_SIZE; i++) {
        MoveToEx(hdc, offsetX + i * CELL_SIZE, offsetY, NULL);
        LineTo(hdc, offsetX + i * CELL_SIZE, offsetY + GRID_SIZE * CELL_SIZE);
    }
    
    // Draw horizontal lines
    for (int i = 1; i < GRID_SIZE; i++) {
        MoveToEx(hdc, offsetX, offsetY + i * CELL_SIZE, NULL);
        LineTo(hdc, offsetX + GRID_SIZE * CELL_SIZE, offsetY + i * CELL_SIZE);
    }
    
    // Draw cells
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            DrawCell(hdc, row, col);
        }
    }
    
    // Clean up
    DeleteObject(framePen);
}

void XOGame::DrawCell(HDC hdc, int row, int col) {
    // Center the board in the window - match DrawBoard
    int offsetX = (WINDOW_WIDTH - (GRID_SIZE * CELL_SIZE)) / 2;
    int offsetY = (WINDOW_HEIGHT - (GRID_SIZE * CELL_SIZE)) / 2 - 60; // Move it a bit higher
    
    RECT cellRect = {
        offsetX + col * CELL_SIZE + 1,
        offsetY + row * CELL_SIZE + 1,
        offsetX + (col + 1) * CELL_SIZE - 1,
        offsetY + (row + 1) * CELL_SIZE - 1
    };
    
    // Draw hover effect
    if (row == m_hoverRow && col == m_hoverCol) {
        FillRect(hdc, &cellRect, m_hoverBrush);
    } else {
        FillRect(hdc, &cellRect, m_cellBrush);
    }
    
    // Draw X or O
    if (m_board[row][col] != CellState::Empty) {
        SelectObject(hdc, m_gameFont);
        SetBkMode(hdc, TRANSPARENT);
        
        #ifdef __GNUC__
            const char* text = (m_board[row][col] == CellState::X) ? "X" : "O";
        #else
            const wchar_t* text = (m_board[row][col] == CellState::X) ? L"X" : L"O";
        #endif
        
        COLORREF textColor = (m_board[row][col] == CellState::X) ? COLOR_X : COLOR_O;
        
        SetTextColor(hdc, textColor);
        
        // Center the text in the cell
        #ifdef __GNUC__
            DrawTextA(hdc, text, -1, &cellRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        #else
            DrawTextW(hdc, text, -1, &cellRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        #endif
    }
}

void XOGame::OnMouseMove(int x, int y) {
    // Check for button hover first
    int prevHoveredButton = m_hoveredButton;
    m_hoveredButton = -1;
    
    for (int i = 0; i < (int)m_buttons.size(); i++) {
        RECT buttonRect = m_buttons[i].first;
        if (x >= buttonRect.left && x <= buttonRect.right && y >= buttonRect.top && y <= buttonRect.bottom) {
            m_hoveredButton = i;
            break;
        }
    }
    
    if (prevHoveredButton != m_hoveredButton) {
        InvalidateRect(m_hwnd, NULL, FALSE);
    }
    
    // Only process board hover in Game screen when game is playing
    if (m_currentScreen == GameScreen::Game && m_gameState == GameState::Playing) {
        // Center the board in the window - match DrawBoard
        int offsetX = (WINDOW_WIDTH - (GRID_SIZE * CELL_SIZE)) / 2;
        int offsetY = (WINDOW_HEIGHT - (GRID_SIZE * CELL_SIZE)) / 2 - 60; // Move it a bit higher
        
        // Adjust for board offset
        int boardX = x - offsetX;
        int boardY = y - offsetY;
        
        // Check if mouse is within grid bounds
        if (boardX >= 0 && boardX < GRID_SIZE * CELL_SIZE && boardY >= 0 && boardY < GRID_SIZE * CELL_SIZE) {
            // Convert to grid coordinates
            int row = boardY / CELL_SIZE;
            int col = boardX / CELL_SIZE;
            
            // Check if cell is empty
            if (m_board[row][col] == CellState::Empty) {
                if (m_hoverRow != row || m_hoverCol != col) {
                    m_hoverRow = row;
                    m_hoverCol = col;
                    InvalidateRect(m_hwnd, NULL, FALSE);
                }
                return;
            }
        }
        
        // If we're here, the hover should be cleared
        if (m_hoverRow != -1 || m_hoverCol != -1) {
            m_hoverRow = -1;
            m_hoverCol = -1;
            InvalidateRect(m_hwnd, NULL, FALSE);
        }
    }
}

void XOGame::OnMouseClick(int x, int y) {
    // Check for button clicks first
    for (int i = 0; i < (int)m_buttons.size(); i++) {
        RECT buttonRect = m_buttons[i].first;
        if (x >= buttonRect.left && x <= buttonRect.right && y >= buttonRect.top && y <= buttonRect.bottom) {
            // Execute the button's action
            m_buttons[i].second();
            return;
        }
    }
    
    // Handle game board clicks in Game screen
    if (m_currentScreen == GameScreen::Game && m_gameState == GameState::Playing) {
        // Center the board in the window - match DrawBoard
        int offsetX = (WINDOW_WIDTH - (GRID_SIZE * CELL_SIZE)) / 2;
        int offsetY = (WINDOW_HEIGHT - (GRID_SIZE * CELL_SIZE)) / 2 - 60; // Move it a bit higher
        
        // Adjust for board offset
        int boardX = x - offsetX;
        int boardY = y - offsetY;
        
        // Calculate which cell was clicked
        int row = boardY / CELL_SIZE;
        int col = boardX / CELL_SIZE;
        
        // Make sure the click is within bounds and the cell is empty
        if (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE && 
            m_board[row][col] == CellState::Empty) {
            
            // Check if current player is human
            bool isCurrentPlayerHuman = (m_currentPlayer == CellState::X) ? 
                (m_xPlayerType == PlayerType::Human) : (m_oPlayerType == PlayerType::Human);
                
            if (!isCurrentPlayerHuman) {
                // Don't allow clicks for AI players
                return;
            }
            
            // Place the player's marker
            m_board[row][col] = m_currentPlayer;
            
            // Check for win or draw
            CheckGameStatus();
            
            // If game ended, show game over screen
            if (m_gameState != GameState::Playing) {
                m_currentScreen = GameScreen::GameOver;
                InvalidateRect(m_hwnd, NULL, FALSE);
                return;
            }
            
            // If game is still going, switch players
            SwitchPlayer();
            
            // AI's turn if current player is AI
            bool isNewPlayerAI = (m_currentPlayer == CellState::X) ? 
                (m_xPlayerType == PlayerType::AI) : (m_oPlayerType == PlayerType::AI);
                
            if (isNewPlayerAI) {
                MakeAIMove();
            }
            
            // Update the display
            UpdateStatusText();
            InvalidateRect(m_hwnd, NULL, FALSE);
        }
    }
}

void XOGame::StartGame(PlayerType xPlayerType, PlayerType oPlayerType) {
    // Store player types
    m_xPlayerType = xPlayerType;
    m_oPlayerType = oPlayerType;
    
    // Reset the game
    ResetGame();
    
    // Switch to game screen
    m_currentScreen = GameScreen::Game;
    
    // Make AI move if X is AI
    if (m_xPlayerType == PlayerType::AI) {
        // Use timer to let UI render first
        SetTimer(m_hwnd, 1, 100, NULL);
    }
    
    // Update the display
    UpdateStatusText();
    InvalidateRect(m_hwnd, NULL, FALSE);
}

void XOGame::ResetGame() {
    // Clear the board
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            m_board[row][col] = CellState::Empty;
        }
    }
    
    // Reset game state
    m_gameState = GameState::Playing;
    m_currentPlayer = CellState::X;
    m_hoverRow = -1;
    m_hoverCol = -1;
    
    UpdateStatusText();
}

void XOGame::CheckGameStatus() {
    // Check for a win
    
    // Check rows
    for (int row = 0; row < GRID_SIZE; row++) {
        if (m_board[row][0] != CellState::Empty && 
            m_board[row][0] == m_board[row][1] && 
            m_board[row][1] == m_board[row][2]) {
            m_gameState = (m_board[row][0] == CellState::X) ? GameState::XWon : GameState::OWon;
            return;
        }
    }
    
    // Check columns
    for (int col = 0; col < GRID_SIZE; col++) {
        if (m_board[0][col] != CellState::Empty && 
            m_board[0][col] == m_board[1][col] && 
            m_board[1][col] == m_board[2][col]) {
            m_gameState = (m_board[0][col] == CellState::X) ? GameState::XWon : GameState::OWon;
            return;
        }
    }
    
    // Check diagonals
    if (m_board[0][0] != CellState::Empty && 
        m_board[0][0] == m_board[1][1] && 
        m_board[1][1] == m_board[2][2]) {
        m_gameState = (m_board[0][0] == CellState::X) ? GameState::XWon : GameState::OWon;
        return;
    }
    
    if (m_board[0][2] != CellState::Empty && 
        m_board[0][2] == m_board[1][1] && 
        m_board[1][1] == m_board[2][0]) {
        m_gameState = (m_board[0][2] == CellState::X) ? GameState::XWon : GameState::OWon;
        return;
    }
    
    // Check for a draw (board full)
    bool boardFull = true;
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            if (m_board[row][col] == CellState::Empty) {
                boardFull = false;
                break;
            }
        }
        if (!boardFull) break;
    }
    
    if (boardFull) {
        m_gameState = GameState::Draw;
    }
}

void XOGame::SwitchPlayer() {
    m_currentPlayer = (m_currentPlayer == CellState::X) ? CellState::O : CellState::X;
}

void XOGame::UpdateStatusText() {
    if (m_gameState == GameState::Playing) {
        // Get player type string
        std::wstring playerTypeStr = (m_currentPlayer == CellState::X) ?
            ((m_xPlayerType == PlayerType::Human) ? L"Human" : L"AI") :
            ((m_oPlayerType == PlayerType::Human) ? L"Human" : L"AI");
        
        // Build status text
        m_statusText = (m_currentPlayer == CellState::X) ? 
                      L"Player X's turn (" + playerTypeStr + L")" : 
                      L"Player O's turn (" + playerTypeStr + L")";
    } else if (m_gameState == GameState::XWon) {
        m_statusText = L"Player X wins!";
    } else if (m_gameState == GameState::OWon) {
        m_statusText = L"Player O wins!";
    } else if (m_gameState == GameState::Draw) {
        m_statusText = L"Game ended in a draw!";
    }
}

void XOGame::MakeAIMove() {
    // Convert our enum to AI player's enum
    AIPlayer::Difficulty aiDifficulty;
    switch (m_aiDifficulty) {
        case AIDifficulty::Easy:
            aiDifficulty = AIPlayer::Difficulty::Easy;
            break;
        case AIDifficulty::Normal:
            aiDifficulty = AIPlayer::Difficulty::Normal;
            break;
        case AIDifficulty::Hard:
            aiDifficulty = AIPlayer::Difficulty::Hard;
            break;
        default:
            aiDifficulty = AIPlayer::Difficulty::Normal;
            break;
    }

    // Get the best move from the AI based on difficulty
    auto [row, col] = m_aiPlayer->GetBestMove(m_board, m_currentPlayer, aiDifficulty);
    
    // Make the move if valid
    if (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE && 
        m_board[row][col] == CellState::Empty) {
        m_board[row][col] = m_currentPlayer;
        
        // Check for win or draw
        CheckGameStatus();
        
        // If game ended, show game over screen
        if (m_gameState != GameState::Playing) {
            m_currentScreen = GameScreen::GameOver;
            InvalidateRect(m_hwnd, NULL, FALSE);
            return;
        }
        
        // If game is still going, switch back to player
        SwitchPlayer();
        
        // If new player is also AI, make another AI move after a short delay
        bool isNewPlayerAI = (m_currentPlayer == CellState::X) ? 
            (m_xPlayerType == PlayerType::AI) : (m_oPlayerType == PlayerType::AI);
            
        if (isNewPlayerAI) {
            // Add a slight delay for better user experience using a timer
            SetTimer(m_hwnd, 1, 500, NULL);
        }
        
        UpdateStatusText();
    }
} 