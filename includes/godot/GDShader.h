#ifndef __SHADER_H__
#define __SHADER_H__

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>

class GDShader {
    private:
        GLuint shaderProgram;

    public:
        GDShader(const GLchar *vertexSourcePath, const GLchar *fragmentSourcePath) {
            std::string *vShaderCode = loadSourceCodeFromFile(vertexSourcePath);
            if( vShaderCode == NULL ) {
                return;
            }
            std::string *fShaderCode = loadSourceCodeFromFile(fragmentSourcePath);
            if( fShaderCode == NULL ) {
                delete []vShaderCode;
                return;
            }
            const char *pVS = vShaderCode->c_str();
            const char *pFS = fShaderCode->c_str();
            
            shaderProgram = loadProgram(pVS, pFS);
            delete vShaderCode;
            delete fShaderCode;

        }
        
        bool isValidProgram() {
            return shaderProgram > 0;
        }

        void use() {
            glUseProgram(shaderProgram);
        }

        GLuint getShaderProgram() {
            return shaderProgram;
        }
        ~GDShader() {
            if( shaderProgram != 0 ) {
                glDeleteProgram(shaderProgram);
            }
        }

    private:
        std::string* loadSourceCodeFromFile(const char* sourcePath) {
            if( sourcePath == NULL ) {
                return NULL;
            }
            std::ifstream shaderFile;//(sourcePath, std::ifstream::in | std::ifstream::binary);
            shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            try{
                
                shaderFile.open(sourcePath);
                if ( !shaderFile.is_open() ) {
                    std::cout << "ERROR::SHADER::OPEN_FILE_FAIL: " << sourcePath << std::endl;
                    return NULL;
                }
                std::ostringstream shaderStream;
                shaderStream << shaderFile.rdbuf();
                shaderFile.close();
                std::string *pSourceCode = new std::string(shaderStream.str());
                return pSourceCode;
            } catch(std::ifstream::failure e) {
                std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n" << e.what() << sourcePath << std::endl;
            }
            return NULL;
        }
        GLuint loadShader(const GLchar *shaderSource, GLenum type) {
            GLuint shader = glCreateShader(type);
            glShaderSource(shader, 1, &shaderSource, NULL);
            glCompileShader(shader);
            GLint success;
            GLchar infoLog[512];
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if( !success ) {
                glGetShaderInfoLog(shader, 512, NULL, infoLog);
                std::cout << "ERROR::FRAGMENT::" << infoLog << "::COPILE_FAILED" << std::endl;
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
                std::cout << "ERROR::FRAGMENT::LINKING_FAILED" << infoLog << std::endl;
                glDeleteProgram(shaderProgram);
                shaderProgram = 0;
            }
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return shaderProgram;
        }
};
#endif