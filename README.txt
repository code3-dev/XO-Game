XO GAME
=======

A modern implementation of the classic Tic-Tac-Toe game with an AI opponent, built using C++ and the Windows API.

FEATURES
--------

* Classic 3x3 Tic-Tac-Toe gameplay
* Multiple AI difficulty levels
* Clean and modern UI
* Play against a friend or AI

REQUIREMENTS
-----------

* Windows OS
* G++ compiler (MinGW or another GCC distribution)
* NSIS (Nullsoft Scriptable Install System) for creating the installer

BUILDING FROM SOURCE
------------------

1. Ensure G++ is installed and added to your system PATH
2. Ensure NSIS is installed (for installer creation)
3. Run the build script:

   ./build.bat

This will:
- Compile the game using g++ with C++17 standard
- Create the executable at build\Release\XOGame.exe
- Generate an installer at build\Release\XOGame_Setup.exe (if NSIS is installed)

INSTALLATION
-----------

Option 1: Direct execution
Run the compiled executable at build\Release\XOGame.exe

Option 2: Installer
Run the installer at build\Release\XOGame_Setup.exe and follow the on-screen instructions.

HOW TO PLAY
----------

1. Launch the game
2. Select game mode (Player vs Player or Player vs AI)
3. For AI mode, select difficulty level
4. Click on the grid to place your mark
5. The game will indicate when a player wins or when there's a draw

PROJECT STRUCTURE
---------------

- main.cpp - Application entry point
- xo_game.h/cpp - Main game logic and UI
- ai_player.h/cpp - AI opponent implementation
- build.bat - Build script
- installer.nsi - NSIS installer script

CONTACT
-------

- Developer: Hossein Pira
- GitHub: https://github.com/code3-dev
- Email: h3dev.pira@gmail.com
- Telegram: @h3dev
- Instagram: @h3dev.pira

LICENSE
-------

See the LICENSE.txt file for details. 