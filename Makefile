# Makefile for Amazons (Amazon Chess)
# Portable version - works with any MinGW-w64 GCC installation
#
# Usage:
#   mingw32-make          Build the project
#   mingw32-make clean    Remove build artifacts
#   mingw32-make run      Build and run
#
# Requirements:
#   - MinGW-w64 (g++.exe, windres.exe, mingw32-make.exe) in PATH

# ---- Compiler settings (override with ?= so user can customize) ----
CXX      ?= g++.exe
WINDRES  ?= windres.exe
# ---- Paths (all relative to this directory) ----
EGE_DIR  = lib
INC_DIR  = $(EGE_DIR)/include
LIB_DIR  = $(EGE_DIR)

# ---- Compiler & Linker flags ----
CXXFLAGS = -I. -I$(INC_DIR) -std=gnu++17 -O2 -pipe \
           -fno-ms-extensions \
           -Werror=return-type -Werror=vla
LDFLAGS  = -L$(LIB_DIR) -lgraphics \
           -luuid -lmsimg32 -lgdi32 -limm32 -lole32 -loleaut32 -lwinmm -lgdiplus \
           -mwindows -Wl,--stack,12582912 -s -static
WINDRESFLAGS =

# ---- Files ----
RES_FILE = Amazons-local_private.res
OBJS     = main.o Game.o Game_board.o AI_Kernel.o Renderer.o SaveManager.o platform/EGEPlatform.o $(RES_FILE)
TARGET   = Amazons-local.exe

# ---- Top-level targets ----
.PHONY: all clean run

all: $(TARGET)

# ---- Link ----
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# ---- Compile C++ sources ----
main.o: main.cpp Game.h Constants.h
	$(CXX) -c main.cpp -o main.o $(CXXFLAGS)

Game.o: Game.cpp Game.h Game_board.h AI_Kernel.h Renderer.h SaveManager.h Constants.h
	$(CXX) -c Game.cpp -o Game.o $(CXXFLAGS)

Game_board.o: Game_board.cpp Game_board.h AI_Kernel.h Constants.h
	$(CXX) -c Game_board.cpp -o Game_board.o $(CXXFLAGS)

AI_Kernel.o: AI_Kernel.cpp AI_Kernel.h
	$(CXX) -c AI_Kernel.cpp -o AI_Kernel.o $(CXXFLAGS)

Renderer.o: Renderer.cpp Renderer.h Game_board.h SaveManager.h Constants.h
	$(CXX) -c Renderer.cpp -o Renderer.o $(CXXFLAGS)

SaveManager.o: SaveManager.cpp SaveManager.h Game_board.h Constants.h
	$(CXX) -c SaveManager.cpp -o SaveManager.o $(CXXFLAGS)

platform/EGEPlatform.o: platform/EGEPlatform.cpp platform/EGEPlatform.h platform/IPlatform.h platform/KeyCode.h
	$(CXX) -c platform/EGEPlatform.cpp -o platform/EGEPlatform.o $(CXXFLAGS)

# ---- Compile Windows resource (icon) ----
$(RES_FILE): Amazons-local_private.rc app.ico
	$(WINDRES) -i Amazons-local_private.rc --input-format=rc -o $(RES_FILE) -O coff $(WINDRESFLAGS)

# ---- Clean (use rm -f for cross-platform compatibility) ----
clean:
	@rm -f main.o Game.o Game_board.o AI_Kernel.o Renderer.o SaveManager.o
	@rm -f platform/EGEPlatform.o
	@rm -f $(RES_FILE)
	@rm -f $(TARGET)

# ---- Run ----
run: $(TARGET)
	./$(TARGET)
