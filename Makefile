CC=g++
CFLAGS=-c -std=c++11 -O2 -DNDEBUG -fexceptions -Wall -Wextra
LDFLAGS=-lSDL2main -lSDL2
SOURCES=$(wildcard src/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXE=img_iter

all: $(SOURCES) $(EXE)

$(EXE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXE) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXE) $(OBJECTS)
