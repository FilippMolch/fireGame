//
// Created by FIlipp on 12.06.2024.
//

#include <HE/HEtools.h>

void fillObj_withoutBuffs(std::unique_ptr<HEobject>& obj, std::unique_ptr<HEobjectDraw>& objDr) {
    obj->scale = glm::vec3(16.0f, 16.0f, 0.0f);
    obj->position = glm::vec3(0.0f);
    obj->rotateVec = glm::vec3(0.0f);
    obj->rotateAngle = 0.0f;
    obj->objShaderID = 0;
    obj->draw = true;
    obj->alphaChUse = false;

    objDr->TEX_2D = 0;
    objDr->indicesSize = 0;
    objDr->primType = GL_TRIANGLES;

    objDr->UCorrectD = 1.0f;
    objDr->UCorrectP = 0.0f;

    objDr->VCorrectD = 1.0f;
    objDr->VCorrectP = 0.0f;

    obj->isTile = false;

    objDr->tileMapWidth = 0;
    objDr->tileMapHeight = 0;

    objDr->vertsSize = 6;

}

void fillObj(std::unique_ptr<HEobject>& obj, std::unique_ptr<HEobjectDraw>& objDr) {

    fillObj_withoutBuffs(obj, objDr);

    static HEvertex verts[] = {
        {glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, 0.5f, 0.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.0f, 1.0f)},

        {glm::vec3(0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(0.5f, 0.5f, 0.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.0f, 1.0f)},
    };

    glGenBuffers(1, &objDr->VBO);
    glGenVertexArrays(1, &objDr->VAO);
    glBindVertexArray(objDr->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, objDr->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(HEvertex) * objDr->vertsSize, verts, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(HEvertex), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(HEvertex), (GLvoid*)offsetof(HEvertex, uv));
    glBindVertexArray(0);

}

void genGLTextureFotText(GLuint* tex, uint32_t w, uint32_t h, uint8_t* buffer) {
    glGenTextures(1, tex);

    glBindTexture(GL_TEXTURE_2D, *tex);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RED,
                     w,
                     h,
                     0,
                     GL_RED,
                     GL_UNSIGNED_BYTE,
                     buffer);

    glBindTexture(GL_TEXTURE_2D, 0);
}