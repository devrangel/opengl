#ifndef MODEL_H
#define MODEL_H

#include <vector>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "shader.h"
#include "mesh.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

unsigned int TextureFromFile(const char *path);

class Model
{
    public:
        // Model(char *buffer, size_t buf_lenght)
        Model(std::string path)
        {
            this->loadModel(path);
        }

        void Draw(Shader &shader)
        {
            for (unsigned int i = 0; i < this->meshes.size(); i++)
            {
                this->meshes[i].Draw(shader);
            }
        }
    
    private:
        // Model data
        std::vector<Mesh> meshes;
        // std::string directory;
        std::vector<Texture> textures_loaded;

        // void loadModel(char *buffer, size_t buf_lenght)
        void loadModel(std::string path)
        {
            Assimp::Importer import;
            // const aiScene *scene = import.ReadFileFromMemory(buffer, buf_lenght, aiProcess_Triangulate | aiProcess_FlipUVs);
            const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

            if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
                return;
            }

            // this->directory = path.substr(0, path.find_last_of('/'));
            // std::cout << this->directory << std::endl;

            processNode(scene->mRootNode, scene);
        }

        void processNode(aiNode *node, const aiScene *scene)
        {
            // Process all the node's meshes (if any)
            for (unsigned int i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
                this->meshes.push_back(processMesh(mesh, scene));
            }

            // Then do the same for each of its children
            for (unsigned int i = 0; i < node->mNumChildren; i++)
            {
                processNode(node->mChildren[i], scene);
            }
        }

        Mesh processMesh(aiMesh *mesh, const aiScene *scene)
        {
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<Texture> textures;
            
            for (unsigned int i = 0; i < mesh->mNumVertices; i++)
            {
                aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
                Vertex vertex;

                // Process vertex positions, colors, normals and texture coordinates
                glm::vec3 vector;

                // Position
                vector.x = mesh->mVertices[i].x;
                vector.y = mesh->mVertices[i].y;
                vector.z = mesh->mVertices[i].z;
                vertex.Position = vector;

                // Ambient color
                aiColor3D ambient;
                if(AI_SUCCESS != material->Get(AI_MATKEY_COLOR_AMBIENT, ambient))
                {
                    std::cout << "Error loading ambient color" << std::endl;
                }
                vector.x = ambient.r;
                vector.y = ambient.g;
                vector.z = ambient.b;
                vertex.Ambient = vector;

                // Diffuse color
                aiColor3D diffuse;
                if(AI_SUCCESS != material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse))
                {
                    std::cout << "Error loading diffuse color" << std::endl;
                }
                vector.x = diffuse.r;
                vector.y = diffuse.g;
                vector.z = diffuse.b;
                vertex.Diffuse = vector;

                // Specular color
                aiColor3D specular;
                if(AI_SUCCESS != material->Get(AI_MATKEY_COLOR_SPECULAR, specular))
                {
                    std::cout << "Error loading specular color" << std::endl;
                }
                vector.x = specular.r;
                vector.y = specular.g;
                vector.z = specular.b;
                vertex.Specular = vector;

                // Shininess
                float shininess = 0.0f;
                if(AI_SUCCESS != material->Get(AI_MATKEY_SHININESS, shininess))
                {
                    std::cout << "Error loading shininess" << std::endl;
                }
                vertex.Shininess = shininess;

                // Normals
                if(mesh->HasNormals())
                {
                    vector.x = mesh->mNormals[i].x;
                    vector.y = mesh->mNormals[i].y;
                    vector.z = mesh->mNormals[i].z;
                    vertex.Normal = vector;
                }

                // Process material
                // Does the mesh contain texture coordinates?
                if(mesh->mTextureCoords[0])
                {
                    glm::vec2 vec;
                    vec.x = mesh->mTextureCoords[0][i].x;
                    vec.y = mesh->mTextureCoords[0][i].y;
                    vertex.TexCoords = vec;
                }
                else
                {
                    vertex.TexCoords = glm::vec2(0.0f, 0.0f);
                }

                vertices.push_back(vertex);
            }

            // Process indices
            for (unsigned int i = 0; i < mesh->mNumFaces; i++)
            {
                aiFace face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; j++)
                {
                    indices.push_back(face.mIndices[j]);
                }
            }

            // Process material
            if(mesh->mMaterialIndex >= 0)
            {
                aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
                
                // Diffuse
                std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
                textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

                // Specular
                std::vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
                textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            }
            
            return Mesh(vertices, indices, textures);
        }

        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
        {
            std::vector<Texture> textures;
            for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
            {
                aiString str;
                mat->GetTexture(type, i, &str);

                bool skip = false;
                for (unsigned int j = 0; j < this->textures_loaded.size(); j++)
                {
                    if(std::strcmp(this->textures_loaded[j].path.c_str(), str.C_Str()) == 0)
                    {
                        textures.push_back(this->textures_loaded[j]);
                        skip = true;
                        break;
                    }
                }
                
                // If texture has not been loaded already, load it
                if(!skip)
                {
                    Texture texture;
                    texture.id = TextureFromFile(str.C_Str());
                    texture.type = typeName;
                    texture.path = str.C_Str();
                    textures.push_back(texture);
                    this->textures_loaded.push_back(texture);
                }
            }

            return textures;
        }
};

// unsigned int TextureFromFile(const char *path, const std::string &directory)
unsigned int TextureFromFile(const char *path)
{
    std::string filename = std::string(path);

    std::cout << filename << std::endl;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

    if(data)
    {
        unsigned int format;
        if(nrComponents == 1)
        {
            format = GL_RED;
        }
        else if(nrComponents == 3)
        {
            format = GL_RGB;
        }
        else if(nrComponents == 4)
        {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

#endif // MODEL_H