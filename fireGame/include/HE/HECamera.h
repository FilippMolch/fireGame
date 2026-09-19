//
// Created by Filipp on 03.01.2024.
//

#ifndef HOMETOWN_ENGINE_HECAMERA_H
#define HOMETOWN_ENGINE_HECAMERA_H

#include <HE/engineClass.h>

class HECamera {
    friend class HEngine;
private:
    GLfloat cameraSpeed;
    GLfloat deltaTime;

    glm::mat4 proj;
    glm::mat4 view;

    glm::mat4 getView();
    glm::mat4 getProj();

    void processKeys(const bool *keys);

    void recalcView();

    unsigned int moveKeys[4];
    bool configureKeys;

    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;

public:

    void setCameraPos(glm::vec3 pos);
    void setCameraFront(glm::vec3 front);
    void setCameraUp(glm::vec3 up);

    glm::vec3 getCameraPos();
    glm::vec3 getCameraFront();
    glm::vec3 getCameraUp();

    std::string cameraName;
    unsigned int id;

    HECamera(GLfloat cameraSpd);

    bool keyboardMove;
    void setKeysForMove(unsigned int UP,
                        unsigned int DOWN,
                        unsigned int LEFT,
                        unsigned int RIGHT);

};

#endif //HOMETOWN_ENGINE_HECAMERA_H
