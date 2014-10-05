Chris Forkner Assignment 05
========================================

Building This Example
---------------------

To build this example just 

>$ cd build 
>$ make

The excutable will be put in bin

The file path for the obj model needs to be added as a runtime argument

E.X. (if you are in the bin directory) 

>$ ./Matrix gameBoard.obj

E.X. (if you are in the build directory)

>$ ../bin/Matrix ../bin/gameBoard.obj

The name of the model created in Blender is gameBoard.obj

Using freeglut.h instead of glut.h

Using assimp for model Loading

