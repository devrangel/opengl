#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#include "shader.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    glm::vec3 Specular;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    float Shininess;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
    public:
        // Mesh data
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
        {
            this->vertices = vertices;
            this->indices = indices;
            this->textures = textures;

            this->setupMesh();
        }

        void Draw(Shader &shader)
        {
            unsigned int diffuseNr = 1;
            unsigned int specularNr = 1;

            for(unsigned int i = 0; i < textures.size(); i++)
            {
                glActiveTexture(GL_TEXTURE0 + i);

                // Retrieve texture number (the N in diffuse_textureN)
                std::string number;
                std::string name = textures[i].type;

                if(name == "texture_diffuse")
                {
                    number = std::to_string(diffuseNr++);
                }
                else if(name == "texture_specular")
                {
                    number = std::to_string(specularNr++);
                }

                shader.setUniformInt((name + number).c_str(), i);
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
            }
            // glActiveTexture(GL_TEXTURE0);

            glUseProgram(shader.ID);
            for (unsigned int i = 0; i < vertices.size(); i++)
            {
                // Set colors
                unsigned int material_ambient = glGetUniformLocation(shader.ID, "material.ambient");
                unsigned int material_diffuse = glGetUniformLocation(shader.ID, "material.diffuse");
                unsigned int material_specular = glGetUniformLocation(shader.ID, "material.specular");
                unsigned int material_shininess = glGetUniformLocation(shader.ID, "material.shininess");
                glUniform3fv(material_ambient, 1, glm::value_ptr(this->vertices.at(i).Ambient));
                glUniform3fv(material_diffuse, 1, glm::value_ptr(this->vertices.at(i).Diffuse));
                glUniform3fv(material_specular, 1, glm::value_ptr(this->vertices.at(i).Specular));
                glUniform1f(material_shininess, this->vertices.at(1).Shininess);
            }
            // glUseProgram(0);
            
            glBindVertexArray(this->VAO);

            // Draw Mesh
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

    private:
        // Render data
        unsigned int VAO, VBO, EBO;
        
        void setupMesh()
        {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

            glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices[0], GL_STATIC_DRAW);

            // Vertex position
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

            // Vertex normals
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

            // Vertex texture coords
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

            glBindVertexArray(0);
        }
};

#endif // MESH_H