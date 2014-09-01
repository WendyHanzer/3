#ifndef SHADERLOADER_H
#define SHADERLOADER_H

#include <string>

class shader
{
 private:
    char *shaderData;
    std::string shaderName;
    std::string shaderFile;
    bool isFragmentShader;
 public:
    shader();
    shader(std::string name, std::string file, bool isFrag);
    ~shader();
    void printStuff();
};

#endif
