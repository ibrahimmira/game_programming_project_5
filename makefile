CXX=clang++
CXXFLAGS=-std=c++11 -arch arm64 -I/opt/homebrew/opt/raylib/include -I./CS3113
LDFLAGS=-L/opt/homebrew/opt/raylib/lib -lraylib \
        -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

SRC=main.cpp $(wildcard CS3113/*.cpp)
BIN=raylib_app

all: $(BIN)

$(BIN): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC) $(LDFLAGS)

run: all
	./$(BIN)

clean:
	rm -f $(BIN)