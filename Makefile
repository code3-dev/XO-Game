CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -DWIN32 -mwindows
LDFLAGS = -lgdi32 -luser32 -lcomctl32
OUTPUT_DIR = build/Release

SOURCES = main.cpp xo_game.cpp ai_player.cpp
EXECUTABLE = $(OUTPUT_DIR)/XOGame.exe
INSTALLER = XOGame_Setup.exe
NSIS = "C:/Program Files (x86)/NSIS/makensis.exe"

.PHONY: all clean installer

all: prepare $(EXECUTABLE)

prepare:
	@mkdir -p $(OUTPUT_DIR)
	@if [ ! -f app.ico ]; then \
		echo "Warning: app.ico not found! The application will use the default Windows icon."; \
		echo "Please provide an app.ico file for a custom icon."; \
		echo ""; \
	fi

$(EXECUTABLE): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Build completed successfully!"
	@echo "Executable is located at: $(EXECUTABLE)"

installer: all
	@echo "Creating installer..."
	@if [ -f $(NSIS) ]; then \
		$(NSIS) installer.nsi; \
		if [ -f $(INSTALLER) ]; then \
			echo "Installer created successfully: $(INSTALLER)"; \
		else \
			echo "Failed to create installer. Check NSIS script for errors."; \
		fi \
	else \
		echo "NSIS not found. Installer not created."; \
		echo "To create an installer, install NSIS and run: makensis installer.nsi"; \
	fi

clean:
	rm -rf build/
	rm -f $(INSTALLER) 