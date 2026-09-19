//
// Created by Filipp on 01.01.2024.
//

#ifndef HOMETOWN_ENGINE_SHADER_H
#define HOMETOWN_ENGINE_SHADER_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum shCodes {
    SH_WRONG_FILE_PATH,
    SH_UNKNOWN_SHADER_TYPE,
    SH_VERT_COMPILE_ERROR,
    SH_FRAG_COMPILE_ERROR,
    SH_PROGRAM_LINK_ERROR,
    SH_OK
};

enum shTypes {
    SH_VERTEX_SHADER,
    SH_FRAGMENT_SHADER,
};

enum uniformTypes {
    UF_VEC3_FLOAT,
    UF_FLOAT,
    UF_VEC3_INT,
    UF_INT
};

struct shaderUnif {
    std::string unifName;

    uniformTypes type;

    union {
        glm::vec3 vec3;
        float f1;
        int i1;
    };
};

class Shader {
public:
    unsigned int id;
    std::string shaderName;

    shCodes loadShader(shTypes shaderType, std::string filePath);
    GLint getUnifLoc(const char* unif);
    shCodes genProgram();
    void use();

    Shader();
    ~Shader();
private:

    GLchar fInfoLog[512];
    GLchar vInfoLog[512];

    GLchar pInfoLog[512];

    GLint vSuccess, fSuccess, pSuccess;
    GLuint fragmentShader, vertexShader;
    GLuint shaderProgram;
    std::stringstream vertStream, fragStream;

    bool fShStatus, vShStatus, pStatus;
    bool shaderDel;
};

#endif //HOMETOWN_ENGINE_SHADER_H
