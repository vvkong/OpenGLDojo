#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void onRender();
bool doInitBeforeGLRenderLoop();
void doReleaseAfterRenderLoop();

GLfloat verticies[][9] = {
    {
        0.5f,  0.0f, 0.0f,
        0.0f,  0.5,  0.0f,
        0.0f, -0.5f, 0.0f,
    },
    {
       -0.5f,  0.0f, 0.0f,
        0.0f,  0.5,  0.0f,
        0.0f, -0.5f, 0.0f,
    },
};

GLuint shaderProgram[2];

const GLchar *vertexShaderSource1 = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
"}\n";
const GLchar *fragmentShaderSource1 = "#version 330 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"    color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n"
;

const GLchar *vertexShaderSource2 = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"out vec4 vertexColor;"
"void main()\n"
"{\n"
"    gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
"    vertexColor = vec4(1.0, 0.0, 1.0, 1.0);"
"}\n";
const GLchar *fragmentShaderSource2 = "#version 330 core\n"
"in vec4 vertexColor;"
"out vec4 color;\n"
"void main()\n"
"{\n"
"    color = vertexColor;"
"}\n"
;

/**
 * 一般当你打算绘制多个物体时，你首先要生成/配置所有的VAO（和必须的VBO及属性指针)，然后储存它们供后面使用。
 * 当我们打算绘制物体的时候就拿出相应的VAO，绑定它，绘制完物体后，再解绑VAO。
 * */
GLuint vbo[2], vao[2];

GLuint loadShader(const GLchar *shaderSource, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if( !success ) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::FRAGMENT::" << type << "::COPILE_FAILED" << std::endl;
        glDeleteShader(shader);
        shader = 0;
    }
    return shader;
}

GLuint loadProgram(const GLchar *vertexSource, const GLchar *fragmentSource) {
    GLuint vertexShader = loadShader(vertexSource, GL_VERTEX_SHADER);
    if( vertexShader == 0 ) {
        return 0;
    }
    GLuint fragmentShader = loadShader(fragmentSource, GL_FRAGMENT_SHADER);
    if( fragmentShader == 0 ) {
        return 0;
    }
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if( !success ) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::FRAGMENT::LINKING_FAILED" << std::endl;
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

bool doInitBeforeGLRenderLoop() {
    shaderProgram[0] = loadProgram(vertexShaderSource1, fragmentShaderSource1);
    shaderProgram[1] = loadProgram(vertexShaderSource2, fragmentShaderSource2);
    if( shaderProgram[0] == 0 || shaderProgram[1] == 0 ) {
        return false;
    }
    // 顶点数组对象(Vertex Array Object, VAO)
    glGenVertexArrays(2, vao);
    // 顶点缓冲对象，GPU内存(通常被称为显存)中储存大量顶点
    glGenBuffers(2, vbo);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    // 绑定vao，任何随后的顶点属性调用都会储存在这个VAO中
    glBindVertexArray(vao[0]);

    // 使用的任何（在GL_ARRAY_BUFFER目标上的）缓冲调用都会用来配置当前绑定的缓冲(VBO)
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    // 复制顶点数组到缓冲中供OpenGL使用，即复制到上述绑定的vbo
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies[0]), verticies[0], GL_STATIC_DRAW);

    // 通过glVertexAttribPointer设置的顶点属性配置，进行的顶点缓冲对象与顶点属性链接
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // 要想使用VAO，要做的只是使用glBindVertexArray绑定VAO。从绑定之后起，应该绑定和配置对应的VBO和属性指针，之后解绑VAO供之后使用
    // 解绑vao
    glBindVertexArray(0);
    // 小心把下面的解绑定放在vao解绑之前，导致属性设置为空
    // 解绑vbo
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // 解绑ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    ///////////////////////////////////////////////////////////////////////////////////////////
    glBindVertexArray(vao[1]);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies[1]), verticies[1], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}

void onRender() {
    glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram[0]);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(vao[0]);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glUseProgram(shaderProgram[1]);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBindVertexArray(vao[1]);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0); // 没必要每次都解绑，可注释掉
}

void doReleaseAfterRenderLoop() {
    glDeleteBuffers(2, vbo);
    glDeleteVertexArrays(2, vao);
    
    glDeleteProgram(shaderProgram[0]);
    glDeleteProgram(shaderProgram[1]);
    
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