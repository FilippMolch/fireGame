//
// Created by Filipp on 02.01.2024.
//

#define STB_IMAGE_IMPLEMENTATION

#include <stb/stb_image.h>
#include <HE/HEmain.h>
#include <HE/HEtools.h>
#include <HE/sceneMng.h>

SceneMng::SceneMng(std::string mngName, unsigned int id) : sceneName(std::move(mngName)),
sceneID(id),
currentCam(0),
backgroundImageSet(false) {

}

static std::string GetBaseDir(const std::string &filepath) {
    if (filepath.find_last_of("/\\") != std::string::npos)
        return filepath.substr(0, filepath.find_last_of("/\\"));
    return "";
}

unsigned int SceneMng::createTile(const std::string& objectName, HEtileMapSize size, int tileX, int tileY) {

    auto obj = std::make_unique<HEobject>();
    auto objDr = std::make_unique<HEobjectDraw>();

    obj->name = objectName;
    obj->id = objects.size() + 1;

    objDr->name = objectName;
    objDr->id = objectsDraw.size() + 1;

    fillObj(obj, objDr);

    float horTile = (float)size.tileMapWidth / 16.0f;
    float verTile = (float)size.tileMapHeight / 16.0f;

    float horOffset = (1.0f / horTile) * (float)tileX;
    float verOffset = (1.0f / verTile) * (float)tileY;

    objDr->UCorrectD = horTile;
    objDr->UCorrectP = horOffset;

    objDr->VCorrectD = verTile;
    objDr->VCorrectP = verOffset;

    obj->isTile = true;
    obj->tileIDX = tileX;
    obj->tileIDY = tileY;

    objDr->tileMapWidth = size.tileMapWidth;
    objDr->tileMapHeight = size.tileMapHeight;

    objDr->objComplete = true;

    objDr->isUIObj = false;

    objectsDraw.push_back(std::move(objDr));
    objects.push_back(std::move(obj));

    unsigned int objID = objects.size();

    hashmapObjs.insert({objectName, objID});

    return objID;
}

void SceneMng::setTileIDs(const std::string& tileName, int tileX, int tileY) {
    auto it = hashmapObjs.find(tileName);
    if (it == hashmapObjs.end())
        return;

    auto& obj = objects[it->second - 1];
    auto& objDr = objectsDraw[it->second - 1];

    if (!obj->isTile)
        return;

    obj->tileIDX = tileX;
    obj->tileIDY = tileY;

    float horTile = (float)objDr->tileMapWidth / 16.0f;
    float verTile = (float)objDr->tileMapHeight / 16.0f;

    float horOffset = (1.0f / horTile) * (float)tileX;
    float verOffset = (1.0f / verTile) * (float)tileY;

    objDr->UCorrectD = horTile;
    objDr->UCorrectP = horOffset;

    objDr->VCorrectD = verTile;
    objDr->VCorrectP = verOffset;
}

unsigned int SceneMng::createEmptyObject(const std::string& objectName) {
    auto obj = std::make_unique<HEobject>();
    auto objDr = std::make_unique<HEobjectDraw>();

    obj->name = objectName;
    obj->id = objects.size() + 1;

    objDr->name = objectName;
    objDr->id = objectsDraw.size() + 1;

    fillObj(obj,objDr);

    objDr->objComplete = true;

    objDr->isUIObj = false;

    objectsDraw.push_back(std::move(objDr));
    objects.push_back(std::move(obj));

    unsigned int objID = objects.size();

    hashmapObjs.insert({objectName, objID});

    return objID;
}

std::unique_ptr<HEobject>& SceneMng::getObject(const std::string& objName) {
    auto it = hashmapObjs.find(objName);
    if (it == hashmapObjs.end())
        return emptyObj;

    auto& obj = objects[it->second - 1];
    return obj;
}

std::unique_ptr<HEobject>& SceneMng::getObjectByID(unsigned int id) {
    if (id <= 0) return emptyObj;
    auto& obj = objects[id-1];

    return obj;
}

unsigned int SceneMng::loadTexture2D(const std::string& textureName, const std::string& texture) {
    const char* filename = texture.c_str();
    int w, h, ch;
    stbi_uc* image = stbi_load(filename, &w, &h, &ch, 0);

    if (!image) return 0;

    auto tex = std::make_unique<HETexture>();
    tex->name = textureName;
    tex->id= textures.size() + 1;
    tex->width = w;
    tex->height = h;

    glGenTextures(1, &tex->GLTexID);

    hashmapTexs.insert({textureName, tex->id});

    glBindTexture(GL_TEXTURE_2D, tex->GLTexID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        GLint tar = ch > 3? GL_SRGB_ALPHA : GL_SRGB;
        GLint format = ch > 3? GL_RGBA : GL_RGB;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     tar,
                     w,
                     h,
                     0,
                     format,
                     GL_UNSIGNED_BYTE,
                     image);

        delete image;

    glBindTexture(GL_TEXTURE_2D, 0);

    textures.push_back(std::move(tex));

    return textures.back()->id;
}

HETexture SceneMng::getTexture(const std::string& textureName) {
    HETexture tex;

    auto it = hashmapTexs.find(textureName);
    if (it == hashmapTexs.end())
        return tex;

    tex = *textures[it->second - 1];
    return tex;
}

HETexture SceneMng::getTextureByID(unsigned int texID) {
    HETexture tex;
    if (texID <= 0) return tex;

    tex = *textures[texID - 1];

    return tex;
}

void SceneMng::deleteTextureByID(unsigned int texID) {
    auto itTex = textures.begin() + (texID - 1);
    auto itHash = hashmapTexs.find(itTex->get()->name);

    if (itHash == hashmapTexs.end() || itTex == textures.end()) return;

    textures.erase(itTex);
    hashmapTexs.erase(itHash);
}

unsigned int SceneMng::setObjectTexture(unsigned int objID, unsigned int texID) {
    if (objID <= 0 || texID <= 0) return 0;

    auto& obj = objectsDraw[objID - 1];
    auto& tex = textures[texID - 1];

    obj->TEX_2D = tex->GLTexID;

    return 1;
}

unsigned int SceneMng::newCamera(const std::string& cameraName, GLfloat cameraSpd) {
    auto cam = std::make_unique<HECamera>(cameraSpd);
    cam->cameraName = cameraName;
    cam->id = cameras.size() + 1;

    cameras.push_back(std::move(cam));
    hashmapCams.insert({cameraName, cameras.size()});

    return cameras.back()->id;
}

std::unique_ptr<HECamera>& SceneMng::getCamera(const std::string& cameraName) {
    auto it = hashmapCams.find(cameraName);
    if (it == hashmapCams.end())
        return emptyCam;

    auto& cam = cameras[it->second - 1];
    return cam;
}

std::unique_ptr<HECamera>& SceneMng::getCameraByID(unsigned int id) {
    if (id <= 0) return emptyCam;
    auto& cam = cameras[id - 1];

    return cam;
}

void SceneMng::bindCamera(unsigned int id) {
    currentCam = id;
}

unsigned int SceneMng::newShader(const std::string& shaderName) {
    auto shd = std::make_unique<Shader>();
    shd->shaderName = shaderName;
    shd->id = shaders.size() + 1;

    shaders.push_back(std::move(shd));
    hashmapShaders.insert({shaderName, shaders.size()});

    return shaders.back()->id;
}

void SceneMng::setShaderUniform(unsigned int shaderID, shaderUnif& unif) {
    std::pair<unsigned int, shaderUnif> unifPair;
    unifPair.first = shaderID;
    unifPair.second = unif;

    shaderUniforms.push_back(std::move(unifPair));
}

std::unique_ptr<Shader>& SceneMng::getShader(const std::string& shaderName) {
    auto it = hashmapShaders.find(shaderName);
    if (it == hashmapShaders.end())
        return emptyShader;

    auto& shd = shaders[it->second - 1];
    return shd;
}

std::unique_ptr<Shader>& SceneMng::getShaderByID(unsigned int id) {
    if (id <= 0) return emptyShader;
    auto& shd = shaders[id - 1];

    return shd;
}

bool SceneMng::setBackGroundImage(const std::string& imageName, const std::string& filePath) {
    if (!backgroundImageSet) {

        backgroundTextureID = loadTexture2D(imageName, filePath);

        backgroundImageShader = newShader("backgroundImageShader");

        static float backgroundQuad[] = {
                // positions   // texCoords
                -1.0f, 1.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f,
                1.0f, -1.0f, 1.0f, 0.0f,

                -1.0f, 1.0f, 0.0f, 1.0f,
                1.0f, -1.0f, 1.0f, 0.0f,
                1.0f, 1.0f, 1.0f, 1.0f
        };

        glGenVertexArrays(1, &back_VAO);
        glGenBuffers(1, &back_VBO);
        glBindVertexArray(back_VAO);
            glBindBuffer(GL_ARRAY_BUFFER, back_VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundQuad), &backgroundQuad, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *) 0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *) (sizeof(GLfloat) * 2));
            glEnableVertexAttribArray(1);
        glBindVertexArray(0);

        auto &shd = getShaderByID(backgroundImageShader);

        if (shd->loadShader(SH_VERTEX_SHADER, "../shaders/backgroundImage.vert") != SH_OK) {
            printf("BACKGROUND_IMAGE VERTEX SHADER: LOAD ERROR\n");
            return false;
        }

        if (shd->loadShader(SH_FRAGMENT_SHADER, "../shaders/backgroundImage.frag") != SH_OK) {
            printf("BACKGROUND_IMAGE FRAGMENT SHADER: LOAD ERROR\n");
            return false;
        }

        shd->genProgram();

        backgroundImageSet = true;

    } else {
        backgroundTextureID = loadTexture2D(imageName, filePath);
    }

    auto& tex = textures[backgroundTextureID - 1];

    static constexpr float renBufAspect = (float)REN_BUF_WIDTH / (float)REN_BUF_HEIGHT;
    float backgroundAspect = (float)tex->width / (float)tex->height;

    backgroundAspect_scaleX = 1.0f / (renBufAspect / backgroundAspect);
    backgroundAspect_scaleY = 1.0f;

    if (backgroundAspect_scaleX > 1.0f) {
        backgroundAspect_scaleX = 1.0f;
        backgroundAspect_scaleY = 1.0f / (backgroundAspect / renBufAspect);
    }

    return true;
}

void SceneMng::deleteBackgroundImage() {
    if (!backgroundImageSet) return;

    deleteTextureByID(backgroundTextureID);

    backgroundImageSet = false;
}
