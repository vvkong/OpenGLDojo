#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "godot/GDCamera.h"
#include "godot/GDShader.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void onRender();
bool doInitBeforeGLRenderLoop();
void doReleaseAfterRenderLoop();

static GDCamera gdCamera(glm::vec3(0.0f, 0.0f, 3.0f));
float vertices[] = {
//     ---- 位置 ----      - 法向量 -
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

static GDShader* gdShader = NULL;
static GLuint vao = 0;
static GLuint vbo = 0;
bool doInitBeforeGLRenderLoop() {
    gdShader = new GDShader("3.1.shader.vs", "3.1.shader.fs");
    if( gdShader == NULL ) {
        return false;
    }
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}

void onRender() {
    glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glm::mat4 eye(1.0f);
    glm::mat4 model(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(gdShader->getShaderProgram(), "model"), 1, GL_FALSE, glm::value_ptr(model));
    glm::mat4 view = gdCamera.getViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(gdShader->getShaderProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)SCR_WIDTH/SCR_HEIGHT, 1.0f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(gdShader->getShaderProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));


    glm::vec3 lightColor; 
    lightColor.x = sin(glfwGetTime() * 2.0f);
    lightColor.y = sin(glfwGetTime() * 0.7f);
    lightColor.z = sin(glfwGetTime() * 1.3f);

    glm::vec3 materialAmbient = lightColor * glm::vec3(0.2f);
    glUniform3fv(glGetUniformLocation(gdShader->getShaderProgram(), "material.ambient"), 1, glm::value_ptr(materialAmbient));
    glm::vec3 materialDiffuse = lightColor * glm::vec3(0.3f);
    glUniform3fv(glGetUniformLocation(gdShader->getShaderProgram(), "material.diffuse"), 1, glm::value_ptr(materialDiffuse));
    glm::vec3 materialSpecular(0.5f, 0.5f, 0.5f);
    glUniform3fv(glGetUniformLocation(gdShader->getShaderProgram(), "material.specular"), 1, glm::value_ptr(materialSpecular));
    
    glUniform1f(glGetUniformLocation(gdShader->getShaderProgram(), "material.shininess"), 32.0f);
    
    
    glm::vec3 lightAmbient(0.2f, 0.2f, 0.2f);
    glUniform3fv(glGetUniformLocation(gdShader->getShaderProgram(), "light.ambient"), 1, glm::value_ptr(lightAmbient));
    glm::vec3 lightDiffuse(0.5f, 0.5f, 0.5f);
    glUniform3fv(glGetUniformLocation(gdShader->getShaderProgram(), "light.diffuse"), 1, glm::value_ptr(lightDiffuse));
    glm::vec3 lightSpecular(1.0f, 1.0f, 1.0f);
    glUniform3fv(glGetUniformLocation(gdShader->getShaderProgram(), "light.specular"), 1, glm::value_ptr(lightSpecular));
    
    glm::vec3 lightPosition(1.0f, 1.0f, 3.0f);
    glUniform3fv(glGetUniformLocation(gdShader->getShaderProgram(), "light.position"), 1, glm::value_ptr(lightPosition));
    

    
    // glm::vec3 lightColor(2.0f, -2.0f, 2.0f);
    // glUniform3fv(glGetUniformLocation(gdShader->getShaderProgram(), "lightColor"), 1, glm::value_ptr(lightColor));
    // glm::vec3 objectColor = glm::vec3(1.0f, 0.5f, 0.31f);
    // glUniform3fv(glGetUniformLocation(gdShader->getShaderProgram(), "objectColor"), 1, glm::value_ptr(objectColor));

    // 灯光位置
    //glm::vec3 lightPos = glm::vec3(2.0f*sin(glfwGetTime()), 2.0f*cos(glfwGetTime()), 6.0f);
    // glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
    // glUniform3fv(glGetUniformLocation(gdShader->getShaderProgram(), "lightPos"), 1, glm::value_ptr(lightPos));

    // 观察者位置
    glm::vec3 viewPos = gdCamera.getPosition();
    glUniform3fv(glGetUniformLocation(gdShader->getShaderProgram(), "viewPos"), 1, glm::value_ptr(viewPos));

    
    glBindVertexArray(vao);
    gdShader->use();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void doReleaseAfterRenderLoop() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(gdShader->getShaderProgram());
    delete gdShader;
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