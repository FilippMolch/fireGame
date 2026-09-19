//
// Created by Filipp on 03.01.2024.
//

#include "HE/HECamera.h"

HECamera::HECamera(GLfloat cameraSpd) {

    proj = glm::ortho(0.0f, (float)REN_BUF_WIDTH, 0.0f, (float)REN_BUF_HEIGHT, -10.0f, 10.0f);

    cameraSpeed = cameraSpd;
    cameraPos   = glm::vec3(0.0f, 0.0f,  0.0f);
    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

    keyboardMove = false;
    configureKeys = false;

    auto callbackLam = [this] (const CameraViewMatRecalcEvent& e) { recalcView(); };

    EventHandler<CameraViewMatRecalcEvent> recalcMat(callbackLam);
    HEevent::Subscribe<CameraViewMatRecalcEvent>(recalcMat);
    HEevent::TriggerEvent(CameraViewMatRecalcEvent());
}

void HECamera::processKeys(const bool *keys) {
    if (!keyboardMove || !configureKeys) return;

    static bool recalcView = false;

    float cameraSpd = cameraSpeed * deltaTime;
    static float cameraSpdLerp = 0.0f;

    static float f = 0.0f;

    static double duration = 1.0f;
    static double startTime = 0.0;
    static bool setStartTime = false;

    if (keys[moveKeys[0]] || keys[moveKeys[1]] || keys[moveKeys[2]] || keys[moveKeys[3]]) {
        if (!setStartTime) {
            setStartTime = true;
            startTime = hEng::getTime();
        }

        recalcView = true;

        f = (float)((hEng::getTime() - startTime) / duration);
        if (f >= 1.0f)
            f = 1.0f;

        cameraSpdLerp = hEng::lerp(0.0f, cameraSpd, f);

    } else {
        f = 0.0f;
        setStartTime = false;
    }

    if(keys[moveKeys[0]])
        cameraPos += cameraSpdLerp * cameraUp;
    if(keys[moveKeys[1]])
        cameraPos -= cameraSpdLerp * cameraUp;
    if(keys[moveKeys[2]])
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpdLerp;
    if(keys[moveKeys[3]])
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpdLerp;

    if (recalcView) {
        HEevent::TriggerEvent(CameraViewMatRecalcEvent());
        recalcView = false;
    }
}

void HECamera::setKeysForMove(unsigned int UP,
                              unsigned int DOWN,
                              unsigned int LEFT,
                              unsigned int RIGHT) {
    moveKeys[0] = UP;
    moveKeys[1] = DOWN;
    moveKeys[2] = LEFT;
    moveKeys[3] = RIGHT;

    configureKeys = true;
}

glm::mat4 HECamera::getProj() {
    return proj;
}

glm::mat4 HECamera::getView() {
    return view;
}

void HECamera::recalcView() {
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

void HECamera::setCameraPos(glm::vec3 pos) {
    cameraPos = pos;
    HEevent::TriggerEvent(CameraViewMatRecalcEvent());
}

void HECamera::setCameraFront(glm::vec3 front) {
    cameraFront = front;
    HEevent::TriggerEvent(CameraViewMatRecalcEvent());
}

void HECamera::setCameraUp(glm::vec3 up) {
    cameraUp = up;
    HEevent::TriggerEvent(CameraViewMatRecalcEvent());
}

glm::vec3 HECamera::getCameraPos() {
    return cameraPos;
}

glm::vec3 HECamera::getCameraFront() {
    return cameraFront;
}

glm::vec3 HECamera::getCameraUp() {
    return cameraUp;
}
