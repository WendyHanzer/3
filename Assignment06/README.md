Chris Forkner Assignment 06
========================================

Building This Example
---------------------

To build this example just 

>$ cd build 
>$ make

The excutable will be put in bin

The file path for the obj model needs to be added as a runtime argument

The EXE requires two arguments: obj file path and texture file path

EXAMPLES:

For capsule (if in bin directory):

>$ ./Matrix /capsule/capsule.obj /capsule/capsule0.jpg

For alduin (if in bin directory):

>$ ./Matrix alduin/alduin.obj alduin/tex/alduin.jpg

There is a provided model with a material file in bin/alduin and bin/capsule

Using freeglut.h instead of glut.h

Using assimp for model Loading

Using Magick++ for textures

