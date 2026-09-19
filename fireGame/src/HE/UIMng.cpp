//
// Created by FIlipp on 11.06.2024.
//

#include <stb/stb_image.h>
#include <HE/UIMng.h>
#include <HE/HEtools.h>
#include <HE/HEmain.h>

UIMng::UIMng(std::string UIname, unsigned int UIID) : name(std::move(UIname)), id(UIID) {
    UIcam = new HECamera(0.0f);
    UIcam->cameraName = "UIcam";
    UIcam->id = 1;

    UIshader = std::make_unique<Shader>();
    TextShader = std::make_unique<Shader>();
    ButtonShader = std::make_unique<Shader>();

    if (UIshader->loadShader(SH_VERTEX_SHADER, "../shaders/UIShader.vert") != SH_OK) printf("UI VERT SHADER LOAD ERROR\n");
    if (UIshader->loadShader(SH_FRAGMENT_SHADER, "../shaders/UIShader.frag") != SH_OK) printf("UI FRAG SHADER LOAD ERROR\n");
    UIshader->genProgram();

    if (TextShader->loadShader(SH_VERTEX_SHADER, "../shaders/TextShader.vert") != SH_OK) printf("TEXT VERT SHADER LOAD ERROR\n");
    if (TextShader->loadShader(SH_FRAGMENT_SHADER, "../shaders/TextShader.frag") != SH_OK) printf("TEXT FRAG SHADER LOAD ERROR\n");
    TextShader->genProgram();

    if (ButtonShader->loadShader(SH_VERTEX_SHADER, "../shaders/ButtonShader.vert") != SH_OK) printf("BUTTON VERT SHADER LOAD ERROR\n");
    if (ButtonShader->loadShader(SH_FRAGMENT_SHADER, "../shaders/ButtonShader.frag") != SH_OK) printf("BUTTON FRAG SHADER LOAD ERROR\n");
    ButtonShader->genProgram();

    shaders.push_back(std::move(UIshader));
    hashmapShaders.insert({"UISHADER", shaders.size()});

    shaders.push_back(std::move(TextShader));
    hashmapShaders.insert({"TEXTSHADER", shaders.size()});

    shaders.push_back(std::move(ButtonShader));
    hashmapShaders.insert({"BUTTONSHADER", shaders.size()});

    if (FT_Init_FreeType(&library))
        printf("FT INIT ERROR\n");

    if (FT_New_Face(library, "../fonts/ARCADECLASSIC.TTF", 0, &face))
        printf("LOAD FONT: fonts/ARCADECLASSIC.TTF -> ERROR\n");

    FT_Set_Char_Size(face, 0, 16*64, 480, 320);
    FT_Set_Pixel_Sizes(face, 0, 200);

    slot = face->glyph;

    focusedButton = 1;

    textCount = 0;
    buttonCount = 0;
    rectCount = 0;

    uiBind = false;

    EventHandler<KeyboardKeyEvent> kUI([this] (const KeyboardKeyEvent& e) {keyBoardEvent(e);});
    HEevent::Subscribe<KeyboardKeyEvent>(kUI);
}

UIMng::~UIMng() {

}

void UIMng::keyBoardEvent(const KeyboardKeyEvent &e) {
    if (!buttonsID.empty() && uiBind) {
        if (e.action == HE_RELEASE && e.key == HE_KEY_W && focusedButton - 1)
            --focusedButton;
        if (e.action == HE_RELEASE && e.key == HE_KEY_S && focusedButton + 1 <= buttonsID.size())
            ++focusedButton;

        if (e.action == HE_RELEASE && e.key == HE_KEY_ENTER) {
            auto it = hashmapButtonsFuncID.find(focusedButton);
            if (it == hashmapButtonsFuncID.end()) return;

            buttonsFunc[it->second - 1]();
        }
    }
}

void UIMng::bindOnClickFunc(unsigned int buttonID, const std::function<void()>& func) {
    auto it = hashmapButtonsFuncID.find(focusedButton - 1);
    if (it != hashmapButtonsFuncID.end()) return;

    buttonsFunc.push_back(func);

    unsigned int funcID = buttonsFunc.size();
    hashmapButtonsFuncID.insert({buttonID, funcID});
}

unsigned int UIMng::addButton(int x, int y, int width, int height) {
    unsigned int objID = objects.size();

    std::string objectName = "UI_BUTTON_NUM_" + std::to_string(buttonCount);
    ++buttonCount;

    auto obj = std::make_unique<HEobject>();
    auto objDr = std::make_unique<HEobjectDraw>();

    obj->name = objectName;
    obj->id = objects.size() + 1;

    objDr->name = objectName;
    objDr->id = objectsDraw.size() + 1;

    fillObj(obj, objDr);

    obj->objShaderID = 3;

    objDr->objComplete = true;
    objDr->isUIObj = true;

    buttonsID.push_back(objID + 1);

    obj->position = glm::vec3(x, y, 0);
    obj->scale = glm::vec3(width, height, 0);

    objects.push_back(std::move(obj));
    objectsDraw.push_back(std::move(objDr));

    return buttonsID.size();
}

unsigned int UIMng::addText(const std::string& text, int x, int y, float scale) {
    std::string objectName = "UI_TEXT_NUM_" + std::to_string(textCount);
    ++textCount;

    auto textObj = std::make_unique<UItext>();

    textObj->text = text;

    for (const char ch: text) {
        std::pair<std::unique_ptr<HEobject>, std::unique_ptr<HEobjectDraw>> textPair;

        auto obj = std::make_unique<HEobject>();
        auto objDr = std::make_unique<HEobjectDraw>();

        obj->name = objectName;
        obj->id = objects.size() + 1;

        objDr->name = objectName;
        objDr->id = objectsDraw.size() + 1;

        fillObj(obj, objDr);

        obj->objShaderID = 2;

        objDr->objComplete = true;
        objDr->isUIObj = true;

        FT_Load_Char(face, ch, FT_LOAD_RENDER);

        GLuint tex = 0;

        unsigned int bitW = slot->bitmap.width, bitR = slot->bitmap.rows;

        genGLTextureFotText(&tex, bitW, bitR, slot->bitmap.buffer);

        objDr->TEX_2D = tex;

        obj->position = glm::vec3(x, y, 0);
        obj->scale = glm::vec3(bitW * scale, bitR * scale, 0);

        x += bitW * scale;

        textPair.first = std::move(obj);
        textPair.second = std::move(objDr);

        textObj->renderText.push_back(std::move(textPair));
    }

    texts.push_back(std::move(textObj));
    unsigned int objID = texts.size();

    return objID;
}

unsigned int UIMng::addRect(int x, int y, int width, int height) {
    std::string objectName = "UI_RECT_NUM_" + std::to_string(rectCount);
    ++rectCount;

    auto obj = std::make_unique<HEobject>();
    auto objDr = std::make_unique<HEobjectDraw>();

    obj->name = objectName;
    obj->id = objects.size() + 1;

    objDr->name = objectName;
    objDr->id = objectsDraw.size() + 1;

    fillObj(obj, objDr);

    obj->objShaderID = 1;

    objDr->objComplete = true;
    objDr->isUIObj = true;

    obj->position = glm::vec3(x, y, 0);
    obj->scale = glm::vec3(width, height, 0);

    objects.push_back(std::move(obj));
    objectsDraw.push_back(std::move(objDr));
    unsigned int objID = objects.size();

    return objID;
}

std::unique_ptr<HEobject>& UIMng::getObject(const std::string& objName) {
    auto it = hashmapObjs.find(objName);
    if (it == hashmapObjs.end())
        return emptyObj;

    auto& obj = objects[it->second - 1];
    return obj;
}

std::unique_ptr<HEobject>& UIMng::getObjectByID(unsigned int id) {
    if (id <= 0) return emptyObj;
    auto& obj = objects[id-1];

    return obj;
}

unsigned int UIMng::newShader(const std::string& shaderName) {
    auto shd = std::make_unique<Shader>();
    shd->shaderName = shaderName;
    shd->id = shaders.size() + 1;

    shaders.push_back(std::move(shd));
    hashmapShaders.insert({shaderName, shaders.size()});

    return shaders.back()->id;
}

void UIMng::setShaderUniform(unsigned int shaderID, shaderUnif& unif) {
    std::pair<unsigned int, shaderUnif> unifPair;
    unifPair.first = shaderID;
    unifPair.second = unif;

    shaderUniforms.push_back(std::move(unifPair));
}

std::unique_ptr<Shader>& UIMng::getShader(const std::string& shaderName) {
    auto it = hashmapShaders.find(shaderName);
    if (it == hashmapShaders.end())
        return emptyShader;

    auto& shd = shaders[it->second - 1];
    return shd;
}

std::unique_ptr<Shader>& UIMng::getShaderByID(unsigned int id) {
    if (id <= 0) return emptyShader;
    auto& shd = shaders[id - 1];

    return shd;
}

unsigned int UIMng::loadTexture2D(const std::string& textureName, const std::string& texture) {
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

HETexture UIMng::getTexture(const std::string& textureName) {
    HETexture tex;

    auto it = hashmapTexs.find(textureName);
    if (it == hashmapTexs.end())
        return tex;

    tex = *textures[it->second - 1];
    return tex;
}

HETexture UIMng::getTextureByID(unsigned int texID) {
    HETexture tex;
    if (texID <= 0) return tex;

    tex = *textures[texID - 1];

    return tex;
}

void UIMng::deleteTextureByID(unsigned int texID) {
    auto itTex = textures.begin() + (texID - 1);
    auto itHash = hashmapTexs.find(itTex->get()->name);

    if (itHash == hashmapTexs.end() || itTex == textures.end()) return;

    textures.erase(itTex);
    hashmapTexs.erase(itHash);
}

unsigned int UIMng::setObjectTexture(unsigned int objID, unsigned int texID) {
    if (objID <= 0 || texID <= 0) return 0;

    auto& obj = objectsDraw[objID - 1];
    auto& tex = textures[texID - 1];

    obj->TEX_2D = tex->GLTexID;

    return 1;
}