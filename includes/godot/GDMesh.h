#ifndef __GDMesh_H__
#define __GDMesh_H__
#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <godot/GDShader.h>
using namespace std;

struct Vertex {
    // 位置
    glm::vec3 position;
    // 法向量
    glm::vec3 normal;
    // 纹理坐标
    glm::vec2 texCoords;
};
struct Texture {
    // 纹理id
    GLuint id;
    // 纹理类型：diffuse、specular
    string type;
    // 纹理路径
    string path;
};
class GDMesh {
private:
    GLuint vao, vbo, ebo;
    
public:
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;

public:
    GDMesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures) {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        this->setupMesh();
    }

    void draw(GDShader &shader) {
        GLuint diffuseNr = 1;
        GLuint specularNr = 1;
        for( GLuint i=0; i<this->textures.size(); i++ ) {
            glActiveTexture(GL_TEXTURE0+i);
            stringstream ss;
            string number;
            string name = this->textures[i].type;
            if( name == "texture_diffuse" ) {
                ss << diffuseNr++;
            } else if( name == "texture_specular" ) {
                ss << specularNr++;
            }
            number = ss.str();

            glUniform1f(glGetUniformLocation(shader.getShaderProgram(), ("material."+name+number).c_str()), i);
            glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
        }
        glActiveTexture(GL_TEXTURE0);

        glBindVertexArray(this->vao);
        glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    void setupMesh() {
        glGenVertexArrays(1, &this->vao);
        glGenBuffers(1, &this->vbo);
        glGenBuffers(1, &this->ebo);
        
        glBindVertexArray(this->vao);
        glBindBuffer(GL_ARRAY_BUFFER, this->vao);
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoords));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
};
#endif