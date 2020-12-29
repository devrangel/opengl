// g++ -g main.cpp ../include/glad/glad.c shader.cpp -I../include -I./src -L../lib -Wall -lglfw3 -lGL -lX11 -lassimp -lpthread -ldl -Wl,-rpath,'$ORIGIN' -o ../run/main

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "utils/shader.h"
#include "utils/camera.h"
#include "utils/model.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "shaders/shader_source.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

// Window settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Light position
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main() 
{
    // GLFW: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // Window object that will hold all data
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL 3.3", NULL, NULL);
    if(window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // GLAD: load all OpenGL function pointers
    // Initialize GLAD before use any OpenGL function
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader ourShader(Source::vert_shader_source, Source::frag_shader_source);

    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------

    // std::ifstream file{"ground.dae"};
    // file.seekg(0, file.end);
    // long length = file.tellg();
    // file.seekg(0, file.beg);

    // char *buffer = new char[length];

    // file.read(buffer, length);
    // file.close();

    // Model ourModel(buffer, length);

    // delete[] buffer;

    Model ourModel("multi.dae"); 

    // Render loop
    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Inputs
        processInput(window);

        // Rendering commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(ourShader.ID);

        glm::vec3 lightColor;
        lightColor.x = sin(glfwGetTime() * 2.0f);
        lightColor.y = sin(glfwGetTime() * 0.7f);
        lightColor.z = sin(glfwGetTime() * 1.3f);

        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = lightColor * glm::vec3(0.2f);

        // Set uniforms
        unsigned int lightPosLoc = glGetUniformLocation(ourShader.ID, "lightPos");
        unsigned int viewPosLoc = glGetUniformLocation(ourShader.ID, "viewPos");
        glUniform3fv(lightPosLoc, 1, &lightPos[0]);
        glUniform3fv(viewPosLoc, 1, &camera.position[0]);

        unsigned int light_ambient = glGetUniformLocation(ourShader.ID, "light.ambient");
        unsigned int light_diffuse = glGetUniformLocation(ourShader.ID, "light.diffuse");
        unsigned int light_specular = glGetUniformLocation(ourShader.ID, "light.specular");
        glUniform3fv(light_ambient, 1, &ambientColor[0]);
        glUniform3fv(light_diffuse, 1, &diffuseColor[0]);
        glUniform3f(light_specular, 1.0f, 1.0f, 1.0f);

        // unsigned int material_ambient = glGetUniformLocation(ourShader.ID, "material.ambient");
        // unsigned int material_diffuse = glGetUniformLocation(ourShader.ID, "material.diffuse");
        // unsigned int material_specular = glGetUniformLocation(ourShader.ID, "material.specular");
        // unsigned int material_shininess = glGetUniformLocation(ourShader.ID, "material.shininess");
        // glUniform3f(material_ambient, 1.0f, 0.5f, 0.31f);
        // glUniform3f(material_diffuse, 1.0f, 0.5f, 0.31f);
        // glUniform3f(material_specular, 1.0f, 0.5f, 0.31f);
        // glUniform1f(material_shininess, 32.0f);

        // view/projections transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.getViewMatrix();
        unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
        unsigned int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

        ourModel.Draw(ourShader);

        // Check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // De-allocate all resources once they have outlived their purpose
    glDeleteProgram(ourShader.ID);
    
    glfwTerminate();
    return 0;
}

// Call this function every time the user resize the window
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Checks if the escape key was pressed and then close the window
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(CameraMovement::FORWARD, deltaTime);
    } 
    else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(CameraMovement::BACKWARD, deltaTime);
    } 
    else if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(CameraMovement::LEFT, deltaTime);
    }  
    else if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(CameraMovement::RIGHT, deltaTime);;
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScrool(yoffset);
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}