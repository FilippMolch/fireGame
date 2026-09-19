//
// Created by FIlipp on 11.06.2024.
//

#ifndef UIMNG_H
#define UIMNG_H

#include <HE/engineClass.h>
#include <ft2build.h>

#include "HEmain.h"

#include FT_FREETYPE_H

typedef struct HEobjectDraw HEobjectDraw;
typedef struct HEobject HEobject;
typedef struct HETexture HETexture;

class HECamera;

class UIMng {
    friend class HEngine;
private:
    std::vector<std::unique_ptr<HEobject>> objects;
    std::vector<std::unique_ptr<HEobjectDraw>> objectsDraw;
    std::unordered_map<std::string, unsigned int> hashmapObjs;

    std::vector<std::unique_ptr<Shader>> shaders;
    std::unordered_map<std::string, unsigned int> hashmapShaders;
    std::vector<std::pair<unsigned int, shaderUnif>> shaderUniforms;

    std::vector<std::unique_ptr<UItext>> texts;

    unsigned int focusedButton;
    std::vector<unsigned int> buttonsID;
    std::vector<std::function<void()>> buttonsFunc;
    std::unordered_map<unsigned int, unsigned int> hashmapButtonsFuncID;

    std::vector<std::unique_ptr<HETexture>> textures;
    std::unordered_map<std::string, unsigned int> hashmapTexs;

    std::string name;
    unsigned int id;

    HECamera* UIcam;

    std::unique_ptr<Shader> UIshader;
    std::unique_ptr<Shader> TextShader;
    std::unique_ptr<Shader> ButtonShader;

    FT_Library library;
    FT_Face face;
    FT_GlyphSlot slot;

    unsigned int textCount;
    unsigned int buttonCount;
    unsigned int rectCount;

    bool uiBind;

    std::unique_ptr<Shader> emptyShader;
    std::unique_ptr<HEobject> emptyObj;

    void keyBoardEvent(const KeyboardKeyEvent& e);

public:
    UIMng(std::string UIname, unsigned int UIID);

    unsigned int addText(const std::string& text, int x, int y, float scale);
    unsigned int addButton(int x, int y, int width, int height);
    unsigned int addRect(int x, int y, int width, int height);

    std::unique_ptr<HEobject>& getObject(const std::string& objName);
    std::unique_ptr<HEobject>& getObjectByID(unsigned int id);

    void bindOnClickFunc(unsigned int buttonID, const std::function<void()>& func);

    unsigned int newShader(const std::string& shaderName);
    void setShaderUniform(unsigned int shaderID, shaderUnif& unif);

    std::unique_ptr<Shader>& getShader(const std::string& shaderName);
    std::unique_ptr<Shader>& getShaderByID(unsigned int id);

    unsigned int loadTexture2D(const std::string& textureName, const std::string& texture);
    HETexture getTexture(const std::string& textureName);
    HETexture getTextureByID(unsigned int texID);
    void deleteTextureByID(unsigned int texID);

    unsigned int setObjectTexture(unsigned int objID, unsigned int texID);

    ~UIMng();
};

#endif //UIMNG_H
