#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "godot/GDCamera.h"
#include "godot/GDShader.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include "stb_image.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void onRender();
bool doInitBeforeGLRenderLoop();
void doReleaseAfterRenderLoop();

GLuint loadTexture(const char *imagePath) {    
    int width, height, nrComponents;
    unsigned char *image = stbi_load(imagePath, &width, &height, &nrComponents, 0);
    if( image == NULL ) {
        std::cout << "load image fail: " << imagePath << std::endl;
        return 0;
    }
    
    GLenum format;
    if (nrComponents == 1)
        format = GL_RED;
    else if (nrComponents == 3)
        format = GL_RGB;
    else if (nrComponents == 4)
        format = GL_RGBA;

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(image);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}


static GDCamera gdCamera(glm::vec3(0.0f, 0.0f, 3.0f));

GLfloat vertices[] = {
    // Positions          // Normals           // Texture Coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};
glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};
glm::vec3 pointLightPositions[] = {
    glm::vec3( 0.7f,  0.2f,  2.0f),
    glm::vec3( 2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3( 0.0f,  0.0f, -3.0f)
};
static GDShader* gdShader = NULL;
GLuint vao = 0;
GLuint vbo = 0;
GLuint diffuseTextureId = 0;
GLuint specTextureId = 0;
GLuint emissionTextureId = 0;
void releaseProgram() {
    if( gdShader != NULL ) {
        glDeleteProgram(gdShader->getShaderProgram());
        delete gdShader;
    }
}
bool doInitBeforeGLRenderLoop() {
    gdShader = new GDShader("6.0.shader.vs", "6.0.shader.fs");
    if( !gdShader->isValidProgram() ) {
        return false;
    }
    diffuseTextureId = loadTexture("../../resources/textures/container2.png");
    if( diffuseTextureId == 0 ) {
        releaseProgram();
        return false;
    }
    // 黑白的镜面贴图，反射高光
    specTextureId = loadTexture("../../resources/textures/container2_specular.png");
    // 不真实的镜面贴图，反射出不同颜色的高光
    //specTextureId = loadTexture("../../resources/textures/container2_specular_colored.png");
    if( specTextureId == 0 ) {
        releaseProgram();
        glDeleteTextures(1, &diffuseTextureId);
        return false;
    }
    // 放射光贴图
    emissionTextureId = loadTexture("../../resources/textures/matrix.jpg");
    if( emissionTextureId == 0 ) {
        releaseProgram();
        glDeleteTextures(1, &diffuseTextureId);
        glDeleteTextures(1, &specTextureId);
        return false;
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}

void onRender() {
    glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
   
    glBindVertexArray(vao);
   
    gdShader->use();
   
    // 激活绑定纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTextureId);
    gdShader->setSampler2D("material.diffuse", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specTextureId);
    gdShader->setSampler2D("material.specular", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, emissionTextureId);
    gdShader->setSampler2D("material.emission", 2);

    glm::vec3 materialSpecular(0.5f, 0.5f, 0.5f);
    gdShader->setVec3("material.specular", materialSpecular);
    gdShader->setFloat("material.shininess", 32.0f);

    // 平行光
    gdShader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    gdShader->setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    gdShader->setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    gdShader->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
    
    // 点光源
    // Point light 1
    gdShader->setVec3( "pointLights[0].position", pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
    gdShader->setVec3( "pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    gdShader->setVec3( "pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    gdShader->setVec3( "pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    gdShader->setFloat( "pointLights[0].constant", 1.0f);
    gdShader->setFloat( "pointLights[0].linear", 0.09);
    gdShader->setFloat( "pointLights[0].quadratic", 0.032);
    // Point light 2
    gdShader->setVec3( "pointLights[1].position", pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
    gdShader->setVec3( "pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    gdShader->setVec3( "pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    gdShader->setVec3( "pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    gdShader->setFloat( "pointLights[1].constant", 1.0f);
    gdShader->setFloat( "pointLights[1].linear", 0.09);
    gdShader->setFloat( "pointLights[1].quadratic", 0.032);
    // Point light 3
    gdShader->setVec3( "pointLights[2].position", pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
    gdShader->setVec3( "pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    gdShader->setVec3( "pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    gdShader->setVec3( "pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    gdShader->setFloat( "pointLights[2].constant", 1.0f);
    gdShader->setFloat( "pointLights[2].linear", 0.09);
    gdShader->setFloat( "pointLights[2].quadratic", 0.032);
    // Point light 4
    gdShader->setVec3( "pointLights[3].position", pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
    gdShader->setVec3( "pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    gdShader->setVec3( "pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    gdShader->setVec3( "pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    gdShader->setFloat( "pointLights[3].constant", 1.0f);
    gdShader->setFloat( "pointLights[3].linear", 0.09);
    gdShader->setFloat( "pointLights[3].quadratic", 0.032);

    // 聚光灯，手电筒
    gdShader->setVec3("spotLight.ambient", 0.2f, 0.2f, 0.2f);
    // gdShader->setVec3("spotLight.diffuse", 0.5f, 0.5f, 0.5f);
    gdShader->setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.5f);
    // gdShader->setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    gdShader->setVec3("spotLight.specular", 0.0f, 0.0f, 1.0f);
    gdShader->setVec3("spotLight.position", gdCamera.position);
    gdShader->setVec3("spotLight.direction", gdCamera.front);
    gdShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    gdShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(25.0f)));


    glm::vec3 viewPos = gdCamera.getPosition();
    gdShader->setVec3("viewPos", viewPos);

    
    glm::mat4 view = gdCamera.getViewMatrix();
    gdShader->setMat4("view", view);
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)SCR_WIDTH/(GLfloat)SCR_HEIGHT, 0.1f, 100.0f);
    gdShader->setMat4("projection", projection);

    glm::mat4 model(1.0f);
    int size = sizeof(cubePositions) / sizeof(cubePositions[0]);
    for(int i = 0;i < size;i++) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        GLfloat angle = 20.0f * i;
        model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
        gdShader->setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0); 
    glBindTexture(GL_TEXTURE_2D, 0);  
}

void doReleaseAfterRenderLoop() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteTextures(1, &diffuseTextureId);
    glDeleteTextures(1, &specTextureId);
    glDeleteTextures(1, &emissionTextureId);
    releaseProgram();
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