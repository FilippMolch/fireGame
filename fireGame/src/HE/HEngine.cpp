//
// Created by Filipp on 01.01.2024.
//

#include <HE/HEmain.h>
#include <HE/HEngine.h>

#include <Windows.h>
#include "HE/engineClass.h"


static std::string GetBaseDir(const std::string &filepath);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void window_focus_callback(GLFWwindow* window, int focused);

HEinit ini;
std::unique_ptr<EventManager> mainEventManager;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

bool keyboardKeys[1024];

void hEng::setInitStruct(struct HEinit&& init) {
    ini = init;
}

double hEng::getTime() {
    return glfwGetTime();
}

GLfloat hEng::getDeltaTime() {
    return deltaTime;
}

bool hEng::getKeyStatus(unsigned int key) {
    return keyboardKeys[key];
}

float hEng::lerp(float a, float b, float f) {
    return a * (1.0f - f) + (b * f);
}

glm::vec3 hEng::lerp(glm::vec3 a, glm::vec3 b, float f) {
    glm::vec3 finalVec;

    finalVec.x = a.x * (1.0f - f) + (b.x * f);
    finalVec.y = a.y * (1.0f - f) + (b.y * f);
    finalVec.z = a.z * (1.0f - f) + (b.z * f);

    return finalVec;
}

HEngine::HEngine() {

    window = nullptr;
    monitor = nullptr;

    resizeFlag = false;

    frameBufInit = false;

    currentSceneID = 0;

    scrRatMat = glm::mat4(1.0f);
    screenWidth = ini.screenWidth;
    screenHeight = ini.screenHeight;

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, ini.resize? GL_TRUE : GL_FALSE);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    if (!width || !height) return;

    glViewport(0, 0, width, height);

    HEngine::instance().screenWidth = width;
    HEngine::instance().screenHeight = height;

    HEevent::TriggerEvent(WindowResizeEvent(width, height));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
    if(action == GLFW_PRESS)
        keyboardKeys[key] = true;
    else if(action == GLFW_RELEASE)
        keyboardKeys[key] = false;

    std::unique_ptr<KeyboardKeyEvent> keyE = std::make_unique<KeyboardKeyEvent>(key, action);
    HEevent::QueueEvent(std::move(keyE));
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    //mouse.x = xpos;
    //mouse.y = ypos;
}

void window_focus_callback(GLFWwindow* window, int focused) {
    HEngine::instance().windowFocus = focused;
}

HEngine::~HEngine(){
    mainEventManager->shutdown();

    glfwTerminate();
}

errorCodes HEngine::createWindow(void(*update)(), void(*start)()) {

    mainEventManager = std::make_unique<EventManager>();

    EventHandler<WindowResizeEvent> resize([this](const WindowResizeEvent& e) {
        scrRatMat = glm::mat4(1.0f);
        WindowAspect = (float)screenWidth / (float)screenHeight;

        scaleX = 1.0f / (WindowAspect / RenBufAspect);
        scaleY = 1.0f;

        if (scaleX > 1.0f) {
            scaleY = 1.0f / (RenBufAspect / WindowAspect);
            scaleX = 1.0f;
        }
        scrRatMat = glm::scale(scrRatMat, glm::vec3(scaleX, scaleY, 1.0f));
    });

    HEevent::Subscribe<WindowResizeEvent>(resize);

    mainEventManager->triggerEvent(WindowResizeEvent(screenHeight, screenWidth));

    upd = update;
    strt = start;

    monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* vMode = glfwGetVideoMode(monitor);

    int width = screenWidth;
    int height = screenHeight;

    if (ini.fullScreen){
        width = vMode->width;
        height = vMode->height;
    }

    window = glfwCreateWindow(width, height, ini.windowTitle.c_str(), ini.fullScreen? monitor : nullptr, nullptr);

    if (!window)
        return HE_ERROR_CREATE_SCREEN;
    glfwMakeContextCurrent(window);

    glfwSetWindowSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return HE_GLAD_INIT_ERROR;
    }

    windowFocus = true;

    glfwSetWindowFocusCallback(window, window_focus_callback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_ALPHA_TEST);

    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    glViewport(0, 0, screenWidth, screenHeight);

    glGenFramebuffers(1, &frameBuf);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuf);
        glGenTextures(1, &renTex);
        glBindTexture(GL_TEXTURE_2D, renTex);
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_RGBA,
                         REN_BUF_WIDTH,
                         REN_BUF_HEIGHT,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renTex, 0);

        glGenRenderbuffers(1, &renBuf);
            glBindRenderbuffer(GL_RENDERBUFFER, renBuf);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, REN_BUF_WIDTH, REN_BUF_HEIGHT);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renBuf);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                printf("ERROR: FRAMEBUFFER NOT COMPLETE\n");
            else
                frameBufInit = true;
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    setCursor(ini.grabCursor);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (defaultShader.loadShader(SH_VERTEX_SHADER, "../shaders/defaultShader.vert") == SH_WRONG_FILE_PATH) printf("VERT defaultShader ERROR FILE PATH \n");
    if (defaultShader.loadShader(SH_FRAGMENT_SHADER, "../shaders/defaultShader.frag") == SH_WRONG_FILE_PATH) printf("FRAG defaultShader ERROR FILE PATH \n");
    defaultShader.genProgram();

    return HE_OK;
}

void HEngine::setCursor(bool cur) {
    cursor = cur;
    glfwSetInputMode(window, GLFW_CURSOR, !cursor? GLFW_CURSOR_NORMAL:GLFW_CURSOR_DISABLED);
}

void HEngine::start() {
    strt();
    if (!frameBufInit) {
        std::cerr << "FRAMEBUFFER NOT COMPLETE \n";
        system("pause");
        ExitProcess(1);
    }

    static float screenQuad[] = {
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
            1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
            1.0f, -1.0f,  1.0f, 0.0f,
            1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &f_VAO);
    glGenBuffers(1, &f_VBO);
    glBindVertexArray(f_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, f_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuad), &screenQuad, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 2));
        glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    frameBufSh.loadShader(SH_VERTEX_SHADER, "../shaders/screenFrameBuf.vert");
    frameBufSh.loadShader(SH_FRAGMENT_SHADER, "../shaders/screenFrameBuf.frag");
    if (frameBufSh.genProgram() != SH_OK) {
        printf("GEN SHADER PROGRAM ERROR\n");
        system("pause");
        ExitProcess(1);
    }

    EventHandler<WindowResizeEvent> res([this](const WindowResizeEvent& e) {
        if (window != nullptr && frameBufInit)
            render();
    });

    HEevent::Subscribe<WindowResizeEvent>(res);

    if (window != nullptr && frameBufInit) {
        while (!glfwWindowShouldClose(window)) {
            render();
            mainEventManager->dispatchEvents();
        }
    }
}

unsigned int HEngine::addSceneMng(const std::string& name) {
    unsigned int id = scnMngs.size() + 1;
    auto scnObj = std::make_unique<SceneMng>(name, id);
    scnMngs.push_back(std::move(scnObj));
    scnMngIDs.insert({name, id});

    return id;
}

std::unique_ptr<SceneMng>& HEngine::getSceneMng(const std::string& name) {
    auto it = scnMngIDs.find(name);

    if (it == scnMngIDs.end())
        return emptyScene;

    auto& scn = scnMngs[it->second - 1];
    return scn;
}

std::unique_ptr<SceneMng>& HEngine::getSceneMngByID(unsigned int id) {
    if (id <= 0) return emptyScene;

    auto& scn = scnMngs[id-1];
    return scn;
}


void HEngine::bindScnMng(unsigned int id) {
    currentSceneID = id;
}

unsigned int HEngine::addUIMng(const std::string& UIName) {
    unsigned int id = UIMngs.size() + 1;
    auto UIObj = std::make_unique<UIMng>(UIName, id);
    UIMngs.push_back(std::move(UIObj));
    UIMngIDs.insert({UIName, id});

    return id;
}

std::unique_ptr<UIMng>& HEngine::getUIMng(const std::string& name) {
    auto it = UIMngIDs.find(name);

    if (it == UIMngIDs.end())
        return emptyUI;

    auto& ui = UIMngs[it->second - 1];
    return ui;
}

std::unique_ptr<UIMng>& HEngine::getUIMngByID(unsigned int id) {
    if (id <= 0) return emptyUI;

    auto& ui = UIMngs[id-1];
    return ui;

}

void HEngine::bindUIMng(unsigned int id) {
    if (id <= 0) {
        currentUIID = 0;
        return;
    }

    currentUIID = id;

    auto& ui = getUIMngByID(id);
    ui->uiBind = true;
}

void HEngine::render() {
    glfwPollEvents();

    glViewport(0, 0, REN_BUF_WIDTH, REN_BUF_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuf);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw();
        upd();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, screenWidth, screenHeight);
    frameBufSh.use();

    glUniformMatrix4fv(frameBufSh.getUnifLoc("screenRatioCorrect"), 1, GL_FALSE, glm::value_ptr(scrRatMat));
    glBindVertexArray(f_VAO);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, renTex);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);

    glfwSwapInterval(1);
    glfwSwapBuffers(window);
}

void HEngine::draw() {
    SceneMng* drawScene = getSceneMngByID(currentSceneID).get();

    UIMng* drawUI = nullptr;
    if (!UIMngs.empty()) {
        drawUI = getUIMngByID(currentUIID).get();
    }

    if (!drawScene) return;

    HECamera* cam = nullptr;
    if (drawScene->currentCam)
        cam = drawScene->getCameraByID(drawScene->currentCam).get();

    glm::mat4 view, proj, model;

    GLfloat currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    model = glm::mat4(1.0f);

    if (drawScene->backgroundImageSet) {
        auto& tex = drawScene->textures[drawScene->backgroundTextureID - 1];

        GLuint BACKGROUND_TEX2D = tex->GLTexID;

        auto& back_shd = drawScene->getShaderByID(drawScene->backgroundImageShader);

        back_shd->use();

        glm::mat4 back_model = glm::mat4(1.0f);
        back_model = glm::scale(back_model, glm::vec3(drawScene->backgroundAspect_scaleX * 3, drawScene->backgroundAspect_scaleY * 3, 1.0f));
        back_model = glm::translate(back_model, glm::vec3(-cam->cameraPos.x / (500.0f * 20.0f) + 0.1f, -cam->cameraPos.y / (300.0f * 20.0f) - 0.4f, 0.0f));

        glDisable(GL_DEPTH_TEST);
        glUniformMatrix4fv(back_shd->getUnifLoc("modelMat"), 1, GL_FALSE, glm::value_ptr(back_model));
        glBindTexture(GL_TEXTURE_2D, BACKGROUND_TEX2D);
            glBindVertexArray(drawScene->back_VAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glEnable(GL_DEPTH_TEST);
    }

    std::vector<std::pair<HEobject, HEobjectDraw>> drawOrder;

    std::vector<HEobject*> objs;
    std::vector<HEobjectDraw*> objsDraw;

    for (auto& objRef : drawScene->objects) {
        objs.push_back(objRef.get());
    }

    for (auto& objRef : drawScene->objectsDraw) {
        objsDraw.push_back(objRef.get());
    }

    if (drawUI) {
        for (auto& objRef : drawUI->objects) {
            objs.push_back(objRef.get());
        }

        for (auto& objRef : drawUI->objectsDraw) {
            objsDraw.push_back(objRef.get());
        }

        for (auto& textRef : drawUI->texts) {
            for (auto& objRef: textRef.get()->renderText) {
                objs.push_back(objRef.first.get());
                objsDraw.push_back(objRef.second.get());
            }
        }
    }

    for (int i = 0; i < objs.size(); ++i) {
        auto& curObj = objs[i];
        auto& curObjDraw = objsDraw[i];

        auto curObjs = std::pair<HEobject, HEobjectDraw>(*curObj, *curObjDraw);

        if (curObj->alphaChUse || curObjDraw->isUIObj)
            drawOrder.push_back(std::move(curObjs));
        else
            drawOrder.insert(drawOrder.begin(), std::move(curObjs));
    }

    auto old_sceneCam = cam;

    cam->deltaTime = deltaTime;

    cam->processKeys(keyboardKeys);

    for (auto objx : drawOrder) {

        auto obj = &objx.first;
        auto objDr = &objx.second;

        if (objDr->isUIObj && drawUI)
            cam = drawUI->UIcam;

        view = cam->getView();
        proj = cam->getProj();

        if (!objDr->objComplete || !obj->draw) continue;

        model = glm::mat4(1.0f);
        model = glm::translate(model, obj->position);
        if (obj->rotateVec != glm::vec3(0.0f))
            model = glm::rotate(model, glm::radians(obj->rotateAngle), obj->rotateVec);
        model = glm::scale(model, obj->scale);

        int vertCount = 0;

        Shader *shader;

        if (!obj->objShaderID)
            shader = &defaultShader;
        if (obj->objShaderID)
            shader = drawScene->getShaderByID(obj->objShaderID).get();
        if (objDr->isUIObj && drawUI)
            shader = drawUI->getShaderByID(obj->objShaderID).get();

        shader->use();

        switch (objDr->primType) {

            case GL_TRIANGLES:
                vertCount = 3;
                break;

            case GL_POINTS:
                vertCount = 1;
                break;

            default:
                break;
        }

        auto lamFind = [=](const std::pair<unsigned int, shaderUnif>& unif) -> bool {
            return shader->id == unif.first;
        };

        std::vector<std::pair<unsigned int, shaderUnif>>::iterator it;

        bool unifFind = false;

        if (!objDr->isUIObj) {
            it = std::find_if(
                    drawScene->shaderUniforms.begin(),
                    drawScene->shaderUniforms.end(),
                    lamFind
            );

            unifFind = it != drawScene->shaderUniforms.end();
        } else {
            it = std::find_if(
                    drawUI->shaderUniforms.begin(),
                    drawUI->shaderUniforms.end(),
                    lamFind
            );

            unifFind = it != drawUI->shaderUniforms.end();
        }

        if (unifFind) {

            switch (it->second.type) {

                case UF_VEC3_FLOAT: {

                    glUniform3f(shader->getUnifLoc(it->second.unifName.c_str()),
                                it->second.vec3.x,
                                it->second.vec3.y,
                                it->second.vec3.z);
                    break;
                }

                case UF_FLOAT: {
                    glUniform1f(shader->getUnifLoc(it->second.unifName.c_str()),
                                it->second.f1);
                    break;
                }

                case UF_VEC3_INT: {
                    glUniform3i(shader->getUnifLoc(it->second.unifName.c_str()),
                                (int)it->second.vec3.x,
                                (int)it->second.vec3.y,
                                (int)it->second.vec3.z);

                    break;
                }

                case UF_INT: {
                    glUniform1i(shader->getUnifLoc(it->second.unifName.c_str()),
                                it->second.i1);
                    break;
                }

                default: {
                    break;
                }
            }

            drawScene->shaderUniforms.erase(it);
        }

        if (objDr->isUIObj)
            glDisable(GL_DEPTH_TEST);

        if (objDr->isUIObj) {
            if (drawUI) {
                if (!drawUI->buttonsID.empty()) {
                    if (obj->name == ("UI_BUTTON_NUM_" + std::to_string(drawUI->focusedButton - 1)))
                        glUniform1i(shader->getUnifLoc("focused"), 1);
                    else
                        glUniform1i(shader->getUnifLoc("focused"), 0);
                }
            }
        }

        glUniform2f(shader->getUnifLoc("resolution"), obj->scale.x, obj->scale.y);

        glUniform1f(shader->getUnifLoc("correct.UCorrectD"),  objDr->UCorrectD);
        glUniform1f(shader->getUnifLoc("correct.UCorrectP"),  objDr->UCorrectP);

        glUniform1f(shader->getUnifLoc("correct.VCorrectD"),  objDr->VCorrectD);
        glUniform1f(shader->getUnifLoc("correct.VCorrectP"),  objDr->VCorrectP);

        glUniform1i(shader->getUnifLoc("texID"), objDr->TEX_2D);
        glUniformMatrix4fv(shader->getUnifLoc("viewMat"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(shader->getUnifLoc("projMat"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(shader->getUnifLoc("modelMat"), 1, GL_FALSE, glm::value_ptr(model));
        glBindTexture(GL_TEXTURE_2D, objDr->TEX_2D);
            glBindVertexArray(objDr->VAO);
                glDrawArrays(objDr->primType, 0, (int)objDr->vertsSize * vertCount);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glEnable(GL_DEPTH_TEST);

        if (objDr->isUIObj)
            cam = old_sceneCam;

        if (drawUI)
            drawUI->uiBind = false;
    }
}

HEngine &HEngine::instance() {
    static HEngine eng;
    return eng;
}

void HEngine::exit() {
    ExitProcess(0);
}
