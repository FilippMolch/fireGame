//
// Created by Filipp on 01.01.2024.
//
#ifndef HOMETOWN_ENGINE_ENGINECLASS_H
#define HOMETOWN_ENGINE_ENGINECLASS_H

#include <memory>
#include <vector>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <HE/shader.h>
#include <HE/sceneMng.h>
#include <HE/UIMng.h>
#include <HE/HEngine.h>

#include <Event/EventManager.h>

#define REN_BUF_WIDTH 480
#define REN_BUF_HEIGHT 320

#define REN_BUF_WIDTH_D 480.0
#define REN_BUF_HEIGHT_D 320.0

class Event;

enum errorCodes {
    HE_GLAD_INIT_ERROR,
    HE_ERROR_CREATE_SCREEN,
    HE_OK
};

typedef struct HEinit HEinit;

class SceneMng;
class UIMng;

struct GLFWwindow;
struct GLFWmonitor;

class HEngine {

    friend class SceneMng;

    friend void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    friend void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
    friend void window_focus_callback(GLFWwindow* window, int focused);

private:

    void(*upd)();
    void(*strt)();

    GLFWwindow* window;
    GLFWmonitor* monitor;

    bool frameBufInit;
    GLuint frameBuf, renBuf, renTex;
    GLuint f_VBO, f_VAO;
    Shader frameBufSh;

    unsigned int currentSceneID;
    std::vector<std::unique_ptr<SceneMng>> scnMngs;
    std::unordered_map<std::string, unsigned int> scnMngIDs;

    unsigned int currentUIID;
    std::vector<std::unique_ptr<UIMng>> UIMngs;
    std::unordered_map<std::string, unsigned int> UIMngIDs;

    Shader defaultShader;

    bool windowFocus;

    glm::mat4 scrRatMat;
    int screenWidth;
    int screenHeight;
    bool resizeFlag;
    //resize vars
    float scaleX, scaleY;
    float WindowAspect;
    static constexpr float RenBufAspect = (float)REN_BUF_WIDTH / (float)REN_BUF_HEIGHT;

    bool cursor;

    std::unique_ptr<SceneMng> emptyScene;
    std::unique_ptr<UIMng> emptyUI;

    HEngine();
    ~HEngine();

    void draw();
    void render();
public:

    static HEngine &instance();

    errorCodes createWindow(void(*update)(), void(*start)());

    unsigned int addSceneMng(const std::string& name);

    std::unique_ptr<SceneMng>& getSceneMng(const std::string& name);
    std::unique_ptr<SceneMng>& getSceneMngByID(unsigned int id);

    void bindScnMng(unsigned int id);

    unsigned int addUIMng(const std::string& UIName);

    std::unique_ptr<UIMng>& getUIMng(const std::string& name);
    std::unique_ptr<UIMng>& getUIMngByID(unsigned int id);

    void bindUIMng(unsigned int id);

    void setCursor(bool cur);
    void start();

    void exit();

    HEngine(const HEngine&) = delete;
    HEngine & operator=(const HEngine&) = delete;

};

namespace hEng {
    void setInitStruct(HEinit&& init);

    double getTime();
    GLfloat getDeltaTime();
    bool getKeyStatus(unsigned int key);

    float lerp(float a, float b, float f);
    glm::vec3 lerp(glm::vec3 a, glm::vec3 b, float f);
}

#endif //HOMETOWN_ENGINE_HENGINE_H
