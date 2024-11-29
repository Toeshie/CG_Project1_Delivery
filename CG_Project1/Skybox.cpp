#include "Skybox.hpp"
#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>



Skybox::Skybox() : textureID(0), VAO(0), VBO(0) {}

Skybox::~Skybox() {
    if (textureID) glDeleteTextures(1, &textureID);
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
}



bool Skybox::initialize(const std::vector<std::string>& faces) {

    const float size = 100.0f;

    float skyboxVertices[] = {
        -size,  size, -size,
        -size, -size, -size,
        size, -size, -size,
        size, -size, -size,
        size,  size, -size,
        -size,  size, -size,

        -size, -size,  size,
        -size, -size, -size,
        -size,  size, -size,
        -size,  size, -size,
        -size,  size,  size,
        -size, -size,  size,


        size, -size, -size,
        size, -size,  size,
        size,  size,  size,
        size,  size,  size,
        size,  size, -size,
        size, -size, -size,



        -size, -size,  size,
        -size,  size,  size,
        size,  size,  size,
        size,  size,  size,
        size, -size,  size,
        -size, -size,  size,



      -size,  size, -size,
      size,  size, -size,
      size,  size,  size,
      size,  size,  size,
      -size,  size,  size,
      -size,  size, -size,

      -size, -size, -size,
      -size, -size,  size,
      size, -size, -size,
      size, -size, -size,
      -size, -size,  size,
        size, -size,  size

    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);



    stbi_set_flip_vertically_on_load(false);



    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);

        if (!data) {
            std::cerr << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            std::cerr << "STB Reason: " << stbi_failure_reason() << std::endl;
            return false;
        }
        std::cout << "Loaded texture: " << faces[i] << " with dimensions: " << width << "x" << height << " and channels: " << nrChannels << std::endl;
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
            0, 
            format,
            width, 
            height, 
            0,
            format, 
            GL_UNSIGNED_BYTE, 
            data
        );
        stbi_image_free(data);
    }
    // Unbind the texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return true;
}



void Skybox::render(const ShaderProgram& shader) {
  
    glDepthFunc(GL_LEQUAL);
    shader.use();
    shader.setUniform("skybox", 0);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}
