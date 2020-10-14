#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "godot/GDCamera.h"
#include "godot/GDShader.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include "SOIL.h"
#include "godot/GDModel.h"
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void onRender();
bool doInitBeforeGLRenderLoop();
void doReleaseAfterRenderLoop();

GLuint loadTexture(const char *imagePath) {
    GLuint texture;
    int w, h;
    unsigned char* image = SOIL_load_image(imagePath, &w, &h, 0, SOIL_LOAD_RGB);
    if( image == NULL ) {
        std::cout << "load image fail: " << imagePath << std::endl;
        return 0;
    }
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

static GDCamera gdCamera(glm::vec3(0.0f, 0.0f, 3.0f));
GDShader* gdShader = NULL;
GDModel* gdModel = NULL;
glm::vec3 pointLightPositions[] = {
    glm::vec3(2.3f, -1.6f, -3.0f),
    glm::vec3(-1.7f, 0.9f, 1.0f)
};
bool doInitBeforeGLRenderLoop() {
    gdShader = new GDShader("shader.vs", "shader.fs");
    if( gdShader == NULL ) {
        return false;
    }
    gdModel = new GDModel("../../resources/objects//nanosuit/nanosuit.obj");
    return true;
}

void onRender() {
    glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    gdShader->use();
    glm::mat4 view = gdCamera.getViewMatrix();
    gdShader->setMat4("view", view);
    glm::mat4 projection = glm::perspective(gdCamera.zoom, (GLfloat)SCR_WIDTH/(GLfloat)SCR_HEIGHT, 0.1f, 100.0f);
    gdShader->setMat4("projection", projection);
    
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.75f, -5.0f)); // Translate it down a bit so it's at the center of the scene
    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
    gdShader->setMat4("model", model);

    gdShader->setVec3("viewPos", gdCamera.position.x, gdCamera.position.y, gdCamera.position.z);

    gdShader->setVec3("pointLights[0].position", pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
    gdShader->setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    gdShader->setVec3("pointLights[0].diffuse", 1.0f, 1.0f, 1.0f);
    gdShader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    gdShader->setFloat("pointLights[0].constant", 1.0f);
    gdShader->setFloat("pointLights[0].linear", 0.009f);
    gdShader->setFloat("pointLights[0].quadratic", 0.0032f);

    gdShader->setVec3("pointLights[0].position",  pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
    gdShader->setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    gdShader->setVec3("pointLights[0].diffuse", 1.0f, 1.0f, 1.0f);
    gdShader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    gdShader->setFloat("pointLights[0].constant", 1.0f);
    gdShader->setFloat("pointLights[0].linear", 0.009f);
    gdShader->setFloat("pointLights[0].quadratic", 0.0032f);
    
    gdModel->draw(*gdShader);
}

void doReleaseAfterRenderLoop() {
    if( gdShader ) {
        delete gdShader;
    }
    if( gdModel ) {
        delete gdModel;
    }
}

bool keys[1024];
void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mode) {
    if( action == GLFW_PRESS ) {
        keys[key] = true;
    } else if( action == GLFW_RELEASE ) {
        keys[key] = false;
    }
}
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
void doMovement() {
    GLfloat curFrame = glfwGetTime();
    deltaTime = curFrame - lastFrame;
    lastFrame = curFrame;
    GLfloat cameraSpeed = 5.0f*deltaTime;
    CameraMovement direction;
    if( keys[GLFW_KEY_W] ) {
        direction = FORWARD;
    }
    if( keys[GLFW_KEY_S] ) {
        direction = BACKWARD;
    }
    if( keys[GLFW_KEY_A] ) {
        direction = LEFT;
    }
    if( keys[GLFW_KEY_F] ) {
        direction = RIGHT;
    }
    gdCamera.pressKeyboard(direction, deltaTime);
}

GLfloat lastX = SCR_WIDTH / 2.0f;
GLfloat lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
void cursorCallback(GLFWwindow* window, double xpos, double ypos) {
    if( firstMouse ) {
        firstMouse = false;
        lastX = xpos;
        lastY = ypos;
    }
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    gdCamera.processMouseMovement(xoffset, yoffset);
}
int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, cursorCallback );

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // do something before render loop
    if( !doInitBeforeGLRenderLoop() ) {
        std::cout << "Failed to call doInitBeforeGLRenderLoop" << std::endl;
        glfwTerminate();
        return -1;
    }

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);

        doMovement();
        
        // render
        // ------
        onRender();
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    doReleaseAfterRenderLoop();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}