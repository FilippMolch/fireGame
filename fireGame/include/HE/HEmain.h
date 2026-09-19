//
// Created by Filipp on 22.02.2024.
//

#ifndef HOMETOWN_ENGINE_HEMAIN_H
#define HOMETOWN_ENGINE_HEMAIN_H

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include <Event/EventManager.h>

typedef struct HEvertex {
    glm::vec3 pos;
    glm::vec2 uv;
} HEvertex;

typedef struct HEobjectDraw {
    unsigned int id;
    std::string name;

    float UCorrectP, VCorrectP;
    float UCorrectD, VCorrectD;

    int tileMapWidth;
    int tileMapHeight;

    GLuint vertsSize;
    GLuint VBO, VAO, IBO, TEX_2D;
    GLuint indicesSize;

    GLint primType;

    bool objComplete;
    bool isUIObj;

} HEobjectDraw;

typedef struct UItext {
    std::string text;
    std::vector<std::pair<std::unique_ptr<struct HEobject>, std::unique_ptr<HEobjectDraw>>> renderText;

} UItext;

#endif //HOMETOWN_ENGINE_HEMAIN_H
