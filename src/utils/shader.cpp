#include "shader.h"

// Constructor
Shader::Shader(const char *vertexSource, const char *fragmentSource)
{
    // 2. Compile shaders and create shader program
    int success;
    char infoLog[512];

    // Vertex Shader
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSource, NULL);
    glCompileShader(vertex);

    // Print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment Shader
    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSource, NULL);
    glCompileShader(fragment);

    // Print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    //glValidateProgram(ID);

    // Print linking errors if any
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // 3. Delete shaders
    //    They are linked into our program and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

// Use/activate the shader program
void Shader::useProgram()
{
    glUseProgram(Shader::ID);
}

// Utility uniform functions
void Shader::setUniformBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(Shader::ID, name.c_str()), (int)value);
}

void Shader::setUniformInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(Shader::ID, name.c_str()), value);
}

void Shader::setUniformFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(Shader::ID, name.c_str()), value);
}

void Shader::setUniformVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(Shader::ID, name.c_str()), 1, &value[0]);
}
void Shader::setUniformVec3(const std::string &name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(Shader::ID, name.c_str()), x, y, z);
}

void Shader::setUniformMatrixMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(Shader::ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}