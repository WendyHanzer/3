#include "shaderLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

shaderLoader::shaderLoader()
{
 // delcare class data
 shaderData = "";

 shaderName = "default";
 shaderFile = "nope";
 FragmentShader = false;
 dataIsSet = false;
}

shaderLoader::shaderLoader(std::string name, std::string file, bool isFrag)
{
 std::stringstream strStream;

 shaderName = name;
 shaderFile = file;
 FragmentShader = isFrag;

 // initalize default to data not found
 dataIsSet = false;

 // retrieve data from file
 std::ifstream fin;
 fin.clear();
 fin.open(shaderFile.c_str());

 if(fin.good())
   {
    dataIsSet = true;
   }

 strStream << fin.rdbuf();

 shaderData = strStream.str();

 fin.close();
}

shaderLoader::~shaderLoader()
{
 
}

void shaderLoader::printStuff()
{
 std::cout<<std::endl<<std::endl;

 std::cout<<shaderData<<std::endl;


 std::cout<<std::endl<<std::endl;
}

bool shaderLoader::isFragmentShader() const
{
 return FragmentShader;
}

bool shaderLoader::isVertexShader() const
{
 return !FragmentShader;
}

const char* shaderLoader::getShaderData()
{
 return shaderData.c_str();
}





