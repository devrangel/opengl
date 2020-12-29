#ifndef SHADER_SOURCE_H
#define SHADER_SOURCE_H

namespace Source
{
    const char *vert_shader_source = R"(#version 330 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aNormal;
    layout(location = 2) in vec2 aTexCoords;

    out vec3 Normal;
    out vec3 FragPos;
    out vec2 TexCoords;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
       gl_Position = projection * view * model * vec4(aPos, 1.0);
       FragPos = vec3(model * vec4(aPos, 1.0));
       Normal = aNormal;
       TexCoords = aTexCoords;
    })";

    const char *frag_shader_source = R"(#version 330 core

    in vec3 Normal;
    in vec3 FragPos;
    in vec2 TexCoords;

    out vec4 FragColor;

    struct Material {
       vec3 ambient;
       vec3 diffuse;
       vec3 specular;
       float shininess;
    };

    struct Light {
       vec3 position;
       vec3 ambient;
       vec3 diffuse;
       vec3 specular;
    };

    uniform Material material;
    uniform Light light;

    uniform sampler2D texture_diffuse1;

    uniform vec3 lightPos;
    uniform vec3 viewPos;

    void main()
    {
       // Ambient
       vec3 ambient = light.ambient * material.ambient;

       // Diffuse
       vec3 norm = normalize(Normal);
       vec3 lightDir = normalize(lightPos - FragPos);
       float diff = max(dot(norm, lightDir), 0.0);
       vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
       // Specular
       vec3 viewDir = normalize(viewPos - FragPos);
       vec3 reflectDir = reflect(-lightDir, norm);
       float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
       vec3 specular = light.specular * (spec * material.specular);

       vec3 result = ambient + diffuse + specular;
       FragColor = texture(texture_diffuse1, TexCoords) * vec4(result, 1.0);
    })";
}


#endif // SHADER_SOURCE_H