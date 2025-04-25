#include <windows.h>
#include "xo_game.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    XOGame game(hInstance);
    return game.Run(nCmdShow);
} 