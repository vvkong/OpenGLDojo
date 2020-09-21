#ifndef __GDCamera_H__
#define __GDCamera_H__
#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

static const GLfloat YAW = -90.0f;
static const GLfloat PITCH = 0.0f;
static const GLfloat SPEED = 3.0f;
static const GLfloat SENSITIVITY = 0.25f;
static const GLfloat ZOOM = 45.0f;

class GDCamera {
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    GLfloat yaw;
    GLfloat pitch;

    GLfloat movementSpeed;
    GLfloat mouseSensitivity;
    GLfloat zoom;

    GDCamera(glm::vec3 position=glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up=glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH):
        front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM) {
            this->position = position;
            this->worldUp = up;
            this->yaw = yaw;
            this->pitch = pitch;
            this->updateCameraVectors();
        }
    GDCamera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch):
        front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM) {
            this->position = glm::vec3(posX, posY, posZ);
            this->worldUp = glm::vec3(upX, upY, upZ);
            this->yaw = yaw;
            this->pitch = pitch;
            this->updateCameraVectors();
        }

    glm::mat4 getViewMatrix() {
        return glm::lookAt(this->position, this->position+this->front, this->up);
    }

    void pressKeyboard(CameraMovement direction, GLfloat deltaTime) {
        GLfloat velocity = this->movementSpeed * deltaTime;
        if( direction == FORWARD ) {
            this->position += this->front * velocity;
        }
        if( direction == BACKWARD ) {
            this->position -= this->front * velocity;
        }
        if( direction == LEFT ) {
            this->position -= this->right * velocity;
        }
        if( direction == RIGHT ) {
            this->position += this->right * velocity;
        }

        // (this->position).y = 0.0f;//保持y坐标不变
    }
    void processMouseMovement(GLfloat xOffset, GLfloat yOffset, GLboolean constainPitch = true) {
        xOffset *= this->mouseSensitivity;
        yOffset *= this->mouseSensitivity;
        this->yaw += xOffset;
        this->pitch += yOffset;
        if( constainPitch ) {
            if( this->pitch > 89.0f ) {
                this->pitch = 89.0f;
            }
            if( this->pitch < -89.0f ) {
                this->pitch = -89.0f;
            }
        } 
        this->updateCameraVectors();
    }
private:
    void updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(this->pitch));
        front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(pitch));
        this->front = glm::normalize(front);
        this->right = glm::normalize(glm::cross(this->front, this->worldUp));
        this->up = glm::normalize(glm::cross(this->right, this->front));
    }
};

#endif
