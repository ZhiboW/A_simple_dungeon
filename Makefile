CXX      = g++
CXXFLAGS = -std=c++11 -Wall -g

# Output Binary Name
BIN      = Dungeon_crawl

# Object Files
OBJ      = main.o map.o saves.o entity.o utils.o

# Phony Targets (commands that aren't files)
.PHONY: all clean

# Default Target
all: $(BIN)

# Link the executable
$(BIN): $(OBJ)
	$(CXX) $(OBJ) -o $(BIN)

# Compile main.cpp
main.o: main.cpp map.h saves.h entity.h utils.h
	$(CXX) -c main.cpp -o main.o $(CXXFLAGS)

# Compile map.cpp
map.o: map.cpp map.h utils.h
	$(CXX) -c map.cpp -o map.o $(CXXFLAGS)

# Compile saves.cpp
saves.o: saves.cpp saves.h map.h entity.h
	$(CXX) -c saves.cpp -o saves.o $(CXXFLAGS)

# Compile entity.cpp
entity.o: entity.cpp entity.h utils.h
	$(CXX) -c entity.cpp -o entity.o $(CXXFLAGS)

# Compile utils.cpp
utils.o: utils.cpp
	$(CPP) -c utils.cpp -o utils.o $(CXXFLAGS)
	
# Clean up build files
clean:
	rm -f $(OBJ) $(BIN)