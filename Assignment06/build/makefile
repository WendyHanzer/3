# Linux
CC=g++
LIBS= -lglut -lGLEW -lGL -lassimp `Magick++-config --cppflags --cxxflags --ldflags --libs`

# Compiler flags
CXXFLAGS= -g -Wall -std=c++0x -c `Magick++-config --cppflags --cxxflags --ldflags --libs`

all: ../bin/Matrix

../bin/Matrix: ../src/main.o ../src/shaderLoader.o
	$(CC) ../src/main.o ../src/shaderLoader.o -o ../bin/Matrix $(LIBS)

../scr/main.o:	main.cpp
	$(CC) $(CXXFLAGS) ../src/main.cpp $(LIBS)

../scr/shaderLoader.o:	shaderLoader.cpp
	$(CC) $(CXXFLAGS) ../src/shaderLoader.cpp $(LIBS)

clean:
	rm -rf *o ../bin/Matrix

