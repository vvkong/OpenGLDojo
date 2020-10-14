#ifndef __GDMODEL_H__
#define __GDMODEL_H__
#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "godot/GDShader.h"
#include "godot/GDMesh.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "SOIL.h"
using namespace std;
class GDModel {
    public:
        GDModel(GLchar *path) {
            this->loadModel(path);
        }

        void draw(GDShader& shader) {
            for( GLuint i=0; i<this->meshes.size(); i++ ) {
                this->meshes[i].draw(shader);
            }
        }

    private:
        vector<GDMesh> meshes;
        string directory;
        vector<Texture> texturesLoaded;

        void loadModel(string path) {
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
            if( !scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode ) {
                cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
                return;
            }
            this->directory = path.substr(0, path.find_last_of('/'));
            this->processNode(scene->mRootNode, scene);
        }

        void processNode(aiNode *node, const aiScene* scene) {
            for( GLuint i=0; i<node->mNumMeshes; i++ ) {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                this->meshes.push_back(this->processMesh(mesh, scene));
            }
            for( GLuint i=0; i<node->mNumChildren; i++ ) {
                processNode(node->mChildren[i], scene);
            }
        }

        GDMesh processMesh(aiMesh* mesh, const aiScene* scene) {
            vector<Vertex> vertices;
            vector<GLuint> indices;
            vector<Texture> textures;
            for( GLuint i=0; i<mesh->mNumVertices; i++ ) {
                Vertex vertex;
                glm::vec3 vector;
                vector.x = mesh->mVertices[i].x;
                vector.y = mesh->mVertices[i].y;
                vector.z = mesh->mVertices[i].z;
                vertex.position = vector;

                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.normal = vector;

                if( mesh->mTextureCoords[0] ) {
                    glm::vec2 v2;
                    v2.x = mesh->mTextureCoords[0][i].x;
                    v2.y = mesh->mTextureCoords[0][i].y;
                    vertex.texCoords = v2;
                } else {
                    vertex.texCoords = glm::vec2(0.0f, 0.0f);
                }
                vertices.push_back(vertex);
            }

            for(GLuint i=0; i<mesh->mNumFaces; i++ ) {
                aiFace face = mesh->mFaces[i];
                for( GLuint j=0; j<face.mNumIndices; j++ ) {
                    indices.push_back(face.mIndices[j]);
                }
            }

            if( mesh->mMaterialIndex >= 0 ) {
                aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
                vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
                textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
                
                vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
                textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            }
            return GDMesh(vertices, indices, textures);
        }

        vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, string typeName) {
            GLsizei size = material->GetTextureCount(type);
            vector<Texture> textures;
            for( GLuint i=0; i<size; i++ ) {
                aiString str;
                material->GetTexture(type, i, &str);

                GLboolean skip = false;
                for( GLuint j=0; j<texturesLoaded.size(); j++ ) {
                    if( std::strcmp(texturesLoaded[j].path.c_str(), str.C_Str()) == 0 ) {
                        skip = true;
                        textures.push_back(texturesLoaded[j]);
                        break;
                    }
                }
                if( !skip ) {
                    Texture texture;
                    texture.id = this->textureFromFile(str.C_Str(), this->directory);
                    texture.type = typeName;
                    texture.path = string(str.C_Str());
                    textures.push_back(texture);
                }
            }
            return textures;
        }

        GLint textureFromFile(const char* path, string directory) {
            string fileName = string(path);
            fileName = directory + '/' + fileName;
            GLuint textureId;
            glGenTextures(1, &textureId);
            int w, h;
            unsigned char* image = SOIL_load_image(fileName.c_str(), &w, &h, NULL, SOIL_LOAD_RGB);
            glBindTexture(GL_TEXTURE_2D, textureId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
            SOIL_free_image_data(image);
            return textureId;
        }
};

#endif