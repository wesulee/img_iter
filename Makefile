CC=g++
CFLAGS=-c -std=c++11 -fexceptions -Wall -Wextra -O2
LDFLAGS=-lSDL2main -lSDL2
DEBUG=-g -Wwrite-strings -Winit-self -Wcast-align -Wcast-qual -Wpointer-arith -Wstrict-aliasing -Wformat=2 -Wmissing-declarations -Wmissing-include-dirs -Wno-unused-parameter -Wuninitialized
SOURCES=src/arg_parser.cpp src/canvas.cpp src/color.cpp src/file_helper.cpp src/image.cpp src/img_iter.cpp src/main.cpp src/poly_mutator.cpp src/polygon.cpp src/shape.cpp src/viewer.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXE=img_iter


all: $(SOURCES) $(EXE)

debug: CFLAGS += $(DEBUG)
debug: all

$(EXE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXE) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXE) $(OBJECTS)
