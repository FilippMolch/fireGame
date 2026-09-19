//
// Created by Filipp on 02.01.2024.
//

#ifndef HOMETOWN_ENGINE_SCENEMNG_H
#define HOMETOWN_ENGINE_SCENEMNG_H

#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>

#include <HE/HECamera.h>

typedef struct HEobject HEobject;
typedef struct HEobjectDraw HEobjectDraw;
typedef struct HETexture HETexture;
typedef struct HEtileMapSize HEtileMapSize;

class HECamera;

class SceneMng {
#ifndef NDEBUG
    friend class testClass;
#endif
    friend class HEngine;
private:
    std::unique_ptr<HEobject> emptyObj;
    std::unique_ptr<HECamera> emptyCam;
    std::unique_ptr<Shader> emptyShader;
    std::unique_ptr<HETexture> emptyTex;

    std::vector<std::unique_ptr<HEobject>> objects;
    std::vector<std::unique_ptr<HEobjectDraw>> objectsDraw;
    std::unordered_map<std::string, unsigned int> hashmapObjs;

    std::vector<std::unique_ptr<HETexture>> textures;
    std::unordered_map<std::string, unsigned int> hashmapTexs;

    std::vector<std::unique_ptr<HECamera>> cameras;
    std::unordered_map<std::string, unsigned int> hashmapCams;

    std::vector<std::unique_ptr<Shader>> shaders;
    std::unordered_map<std::string, unsigned int> hashmapShaders;
    std::vector<std::pair<unsigned int, shaderUnif>> shaderUniforms;

    unsigned int currentCam;

    float backgroundAspect_scaleX;
    float backgroundAspect_scaleY;

    bool backgroundImageSet;
    unsigned int backgroundTextureID;
    unsigned int backgroundImageShader;

    GLuint back_VAO, back_VBO;

public:
    std::string sceneName;
    unsigned int sceneID;

    explicit SceneMng(std::string mngName, unsigned int id);
    ~SceneMng() = default;

    bool setBackGroundImage(const std::string& imageName, const std::string& filePath);
    void deleteBackgroundImage();

    unsigned int createTile(const std::string& objectName, HEtileMapSize size, int tileX, int tileY);
    void setTileIDs(const std::string& tileName, int tileX, int tileY);

    unsigned int createEmptyObject(const std::string& objectName);

    std::unique_ptr<HEobject>& getObject(const std::string& objName);
    std::unique_ptr<HEobject>& getObjectByID(unsigned int id);

    unsigned int loadTexture2D(const std::string& textureName, const std::string& texture);
    HETexture getTexture(const std::string& textureName);
    HETexture getTextureByID(unsigned int texID);
    void deleteTextureByID(unsigned int texID);

    unsigned int setObjectTexture(unsigned int objID, unsigned int texID);

    unsigned int newCamera(const std::string& cameraName, float cameraSpd);

    std::unique_ptr<HECamera>& getCamera(const std::string& cameraName);
    std::unique_ptr<HECamera>& getCameraByID(unsigned int id);

    void bindCamera(unsigned int id);

    unsigned int newShader(const std::string& shaderName);
    void setShaderUniform(unsigned int shaderID, shaderUnif& unif);

    std::unique_ptr<Shader>& getShader(const std::string& shaderName);
    std::unique_ptr<Shader>& getShaderByID(unsigned int id);

};

#endif //HOMETOWN_ENGINE_SCENEMNG_H
