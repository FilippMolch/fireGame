//
// Created by Filipp on 01.01.2024.
//
#include "HE/shader.h"

Shader::Shader(){
    fShStatus = false;
    vShStatus = false;
    pStatus = false;
    shaderDel = true;
}

Shader::~Shader(){
    if (!shaderDel) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
    if (pStatus) {
        glDeleteProgram(shaderProgram);
    }
}

shCodes Shader::loadShader(shTypes shaderType, std::string filePath) {

    std::ifstream shdr;
    shdr.open(filePath.c_str());

    if(!shdr) return SH_WRONG_FILE_PATH;

    switch (shaderType) {

        case shTypes::SH_VERTEX_SHADER: {
            vertStream << shdr.rdbuf();

            std::string vertexShader_s = vertStream.str();
            const GLchar* vertexShader_s_c = vertexShader_s.c_str();

            vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &vertexShader_s_c, nullptr);

            glCompileShader(vertexShader);
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vSuccess);
            if (!vSuccess){
                glGetShaderInfoLog(vertexShader, 512, nullptr, vInfoLog);
                printf("%s \n", vInfoLog);
                return SH_VERT_COMPILE_ERROR;
            }

            vShStatus = true;
            shdr.close();
            break;
        }

        case shTypes::SH_FRAGMENT_SHADER: {
            fragStream << shdr.rdbuf();

            std::string fragmentShader_s = fragStream.str();
            const GLchar* fragmentShader_s_c = fragmentShader_s.c_str();

            fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &fragmentShader_s_c, nullptr);

            glCompileShader(fragmentShader);
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fSuccess);
            if (!fSuccess){
                glGetShaderInfoLog(fragmentShader, 512, nullptr, fInfoLog);
                printf("%s \n", vInfoLog);
                return SH_FRAG_COMPILE_ERROR;
            }

            fShStatus = true;
            shdr.close();
            break;
        }

        default: {
            shdr.close();
            return SH_UNKNOWN_SHADER_TYPE;
        }
    }

    if (vShStatus && fShStatus){
        shaderDel = false;
    }

    shdr.close();
    return SH_OK;
}

shCodes Shader::genProgram(){
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &pSuccess);
    if (!pSuccess){
        glGetProgramInfoLog(shaderProgram, 512, nullptr, pInfoLog);
        printf("%s \n", pInfoLog);
        return SH_PROGRAM_LINK_ERROR;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    pStatus = true;
    shaderDel = true;

    return SH_OK;
}

GLint Shader::getUnifLoc(const char* unif) {
    return glGetUniformLocation(shaderProgram, unif);
}


void Shader::use(){
    if (pStatus)
        glUseProgram(shaderProgram);
}