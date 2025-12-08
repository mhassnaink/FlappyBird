CXX      = g++
CXXFLAGS = -Iinclude -std=c++20 -Wall # download cgame from https://github.com/mhassnaink/cgame/releases
SRC      = src/main.cpp
LDFLAGS  = -lopengl32 -lgdi32 -lgdiplus -lws2_32 -lmsimg32 -municode -lwinmm -mwindows
TARGET    = Game
FILE      = build/Game.exe
all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(FILE) $(LDFLAGS)

clean:
	rm -f $(FILE)

custom:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(FILE) $(LDFLAGS)
	cd build && ./$(TARGET).exe
run:
	cd build && ./$(TARGET).exe
