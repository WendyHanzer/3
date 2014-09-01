#include "shaderLoader.h"
#include <iostream>
#include <string>

shader::shader()
{
 shaderData = new char[500];
 shaderName = "default";
 shaderFile = "nope";
 isFragmentShader = false;
}

shader::shader(std::string name, std::string file, bool isFrag)
{
 shaderData = new char[500];
 shaderName = name;
 shaderFile = file;
 isFragmentShader = isFrag;
}

shader::~shader()
{
 delete [] shaderData;
}

void shader::printStuff()
{
 std::cout<<"this is a test"<<std::endl;
}
