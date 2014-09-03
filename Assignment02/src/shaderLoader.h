#ifndef SHADERLOADER_H
#define SHADERLOADER_H

#include <string>

class shaderLoader
{
 private:
    std::string shaderData;
    std::string shaderName;
    std::string shaderFile;
    bool FragmentShader;
    bool dataIsSet;
 public:
    shaderLoader();
    shaderLoader(std::string name, std::string file, bool isFrag);
    ~shaderLoader();
    void printStuff();
    bool isFragmentShader() const;
    bool isVertexShader() const;
    const char* getShaderData();
};

#endif
