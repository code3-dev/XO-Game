@echo off
echo Building XO Game...

:: Check if g++ is installed
where g++ >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo g++ compiler not found. Please install MinGW or another GCC distribution.
    exit /b 1
)

:: Check if app.ico exists
if not exist app.ico (
    echo Warning: app.ico not found! The application will use the default Windows icon.
    echo Please provide an app.ico file for a custom icon.
    echo.
)

:: Create output directories
if not exist build mkdir build
if not exist build\Release mkdir build\Release

:: Compile resource file
echo Compiling resources...
windres resources.rc -O coff -o resources.res

:: Compile the application including resources
echo Compiling with g++...
g++ -std=c++17 -O2 -Wall -DWIN32 -mwindows -o build\Release\XOGame.exe main.cpp xo_game.cpp ai_player.cpp resources.res -lgdi32 -luser32 -lcomctl32 -lmsimg32

echo.
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    goto end
) else (
    echo Build completed successfully!
    echo Executable is located at: build\Release\XOGame.exe
    
    echo.
    echo Creating installer...
    if exist "C:\Program Files (x86)\NSIS\makensis.exe" (
        "C:\Program Files (x86)\NSIS\makensis.exe" installer.nsi
        if exist "build\Release\XOGame_Setup.exe" (
            echo Installer created successfully: build\Release\XOGame_Setup.exe
        ) else (
            echo Failed to create installer. Check NSIS script for errors.
        )
    ) else (
        echo NSIS not found. Installer not created.
        echo To create an installer, install NSIS and run: makensis installer.nsi
    )
)

:end
echo.
echo Press any key to exit...
pause > nul 