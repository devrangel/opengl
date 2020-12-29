#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "glad/glad.h"
#include "glm/glm.hpp"

class Shader
{
public:
    // The program ID
    unsigned int ID;

    // Constructor reads and builds the shader
    Shader(const char *vertexSource, const char *fragmentSource);

    // Use/activate the shader program
    void useProgram();

    // Utility uniform function
    void setUniformBool(const std::string &name, bool value) const;
    void setUniformInt(const std::string &name, int value) const;
    void setUniformFloat(const std::string &name, float value) const;
    void setUniformVec3(const std::string &name, const glm::vec3 &value) const;
    void setUniformVec3(const std::string &name, float x, float y, float z) const;
    void setUniformMatrixMat4(const std::string &name, const glm::mat4 &mat) const;
};

#endif // SHADER_H