#ifndef CAMERA_H
#define CAMERA_H

#include <vector>

// #include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

enum class CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.05f;
const float ZOOM = 45.0f;

class Camera
{
public:
    // Camera attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    // Euler angles
    float yaw;
    float pitch;

    // Camera options
    float movementSpeed;
    float mouseSensitivity;
    float zoom;
    
    // With vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
        : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
    {
        this->position = position;
        this->worldUp = up;
        this->yaw = yaw;
        this->pitch = pitch;
        
        this->updateCameraVectors();
    }

    // With scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
    {
        this->position = glm::vec3(posX, posY, posZ);
        this->worldUp = glm::vec3(upX, upY, upZ);
        this->yaw = yaw;
        this->pitch = pitch;
        
        this->updateCameraVectors();
    }

    glm::mat4 getViewMatrix()
    {
        return glm::lookAt(this->position, this->position + this->front, this->up);
    }

    // Keyboard input
    void ProcessKeyboard(CameraMovement direction, float deltaTime)
    {
        float velocity = movementSpeed * deltaTime;

        if(direction == CameraMovement::FORWARD)
        {
            this->position += this->front * velocity;
        }
        if(direction == CameraMovement::BACKWARD)
        {
            this->position -= this->front * velocity;
        }
        if(direction == CameraMovement::LEFT)
        {
            this->position -= this->right * velocity;
        }
        if(direction == CameraMovement::RIGHT)
        {
            this->position += this->right * velocity;
        }
    }

    // Mouse input
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        this->yaw += xoffset;
        this->pitch += yoffset;

        // Constrains pitch value
        if(constrainPitch)
        {
            if(this->pitch > 89.0f)
            {
                this->pitch = 89.0f;
            }
            if(this->pitch < -89.0f)
            {
                this->pitch = -89.0f;
            }
        }

        updateCameraVectors();
    }

    // Mouse scrool input
    void ProcessMouseScrool(float yoffset)
    {
        this->zoom -= (float)yoffset;
        if(this->zoom < 1.0f)
        {
            this->zoom = 1.0f;
        }
        if(this->zoom > 45.0f)
        {
            this->zoom = 45.0f;
        }
    }

private:
    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
        front.y = sin(glm::radians(this->pitch));
        front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

        this->front = glm::normalize(front);

        this->right = glm::normalize(glm::cross(this->front, this->worldUp));
        this->up = glm::normalize(glm::cross(this->right, this->front));
    }
};

#endif  // CAMERA_H