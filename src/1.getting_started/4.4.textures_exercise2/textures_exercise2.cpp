#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "SOIL.h"
#include "godot/GDShader.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void onRender();
bool doInitBeforeGLRenderLoop();
void doReleaseAfterRenderLoop();

GLuint loadTexture(const char *imagePath, GLint wrapSValue, GLint wrapTValue) {
    GLuint texture;
    int w, h;
    unsigned char* image = SOIL_load_image(imagePath, &w, &h, 0, SOIL_LOAD_RGB);
    if( image == NULL ) {
        std::cout << "load image fail: " << imagePath << std::endl;
        return 0;
    }
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapSValue);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapTValue);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}
GLfloat vertices[] = {
//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   2.0f, 2.0f,   // 右上
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f,   // 右下
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 2.0f    // 左上
};
GLuint indices[] = {  // Note that we start from 0!
    0, 1, 3, // First Triangle
    1, 2, 3  // Second Triangle
};

GLuint vao, vbo, ebo;
GLuint textures[2] = {0, 0};

GDShader* gdShader = NULL;
bool doInitBeforeGLRenderLoop() {
    gdShader = new GDShader("4.4.shader.vs", "4.4.shader.fs");
    if( !gdShader->isValidProgram() ) {
        return false;
    }  
    
    textures[0] = loadTexture("../../resources/textures/container.jpg", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    if( textures[0] == 0 ) {
        return false;
    }
    textures[1] = loadTexture("../../resources/textures/awesomeface.png", GL_REPEAT, GL_REPEAT);
    if( textures[1] == 0 ) {
        return false;
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*(sizeof(GLfloat)), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*(sizeof(GLfloat)), (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8*(sizeof(GLfloat)), (GLvoid*)(6*sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}

GLfloat alpha = 0.00;
GLfloat delta = 0.01;
void onRender() {
    glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glUniform1i(glGetUniformLocation(gdShader->getShaderProgram(), "ourTexture1"), 0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glUniform1i(glGetUniformLocation(gdShader->getShaderProgram(), "ourTexture2"), 1);

    alpha += delta;
    if( alpha > 1 ) {
        alpha = 1;
    } else if( alpha < 0 ) {
        alpha = 0;
    }
    if( alpha == 1 || alpha == 0) {
        delta = -delta;
    }
    glUniform1f(glGetUniformLocation(gdShader->getShaderProgram(), "alpha"), alpha);

    glBindVertexArray(vao);
    gdShader->use();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}

void doReleaseAfterRenderLoop() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteTextures(2, textures);
    if( gdShader != NULL ) {
        delete gdShader;
    }

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