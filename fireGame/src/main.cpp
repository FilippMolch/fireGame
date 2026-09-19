#include <iostream>
#include <HE/HEngine.h>

#include <box2d/box2d.h>

#define CURRENT_SCENE auto& currentScene = HEngine::instance().getSceneMngByID(sceneID);

void start();
void update();
void keyCallback(const KeyboardKeyEvent& e);

void createPauseMenu();
void createMainMenu();
void createGameOver();
void createVictory();
void createFinish(float posX, float posY);
void createPlayer(float posX, float posY);
void createLevel1();
void createLevel2();
void createLevel3();
void createStaticTile(int tileIDX, int tileIDY, float tileCoordX, float tileCoordY);
void createTile(int tileIDX, int tileIDY, float tileCoordX, float tileCoordY);
void fireMove();
void createWater(float posX, float posY, float w, float h);

const float physSCALE = 40.0f;
const b2Vec2 physSCALEvec = b2Vec2(16.0f, 16.0f);

float timeStep = 1.0f / 60.0f;

int32 velocityIterations = 8;
int32 positionIterations = 3;

b2Vec2 gravity(0.0f, -10.0f);
b2World world[3] = {b2World(gravity), b2World(gravity), b2World(gravity)};

unsigned int sceneID = 0;

unsigned int mainMenuSceneID = 0;

unsigned int level1 = 0;
unsigned int level2 = 0;
unsigned int level3 = 0;

unsigned int currentLevel = 0;

unsigned int camID = 0;

unsigned int pauseMenu = 0;
unsigned int mainMenuUI = 0;
unsigned int gameOver = 0;
unsigned int victory = 0;

unsigned int rectShaderID = 0;

unsigned int tileMapTexID = 0;
HEtileMapSize tileMapSize;

unsigned int finishID = 0;
unsigned int finishTEXID = 0;
glm::vec2 finishPos;
void* finishText = nullptr;

unsigned int fire = 0;
unsigned int fireShID = 0;
b2CircleShape playerShape[3];
b2BodyDef playerBodyDef[3];
b2Body* playerBody[3];
b2FixtureDef playerFix[3];

unsigned int waterShID = 0;

unsigned int tilesCount = 0;
std::vector<b2BodyDef*> staticTilesBodiesDefs;
std::vector<b2Body*> staticTilesBodies;
std::vector<b2PolygonShape*> staticTilesPolygonShape;
std::vector<b2FixtureDef*> staticTilesFixturesDefs;

#define MAIN_MENU 0
#define PLAY 1
#define PAUSE 2
#define WIN 3
#define GAME_OVER 4

#define LEVEL_1 11
#define LEVEL_2 12
#define LEVEL_3 13

unsigned int gameState = MAIN_MENU;

int main(int argc, char* argv[]){

    HEinit initStruct = {
            1920,
            1080,
            false,
            true,
            false,
            "FireGame"
    };

    hEng::setInitStruct(std::move(initStruct));

    switch (HEngine::instance().createWindow(update, start)) {
        case HE_ERROR_CREATE_SCREEN: {
            std::cerr << "HOMETOWN ENGINE ERROR: HE_ERROR_CREATE_SCREEN \n";
            return 1;
        }

        case HE_GLAD_INIT_ERROR: {
            std::cerr << "HOMETOWN ENGINE ERROR: OPENGL INIT ERROR \n";
            return 1;
        }

        case HE_OK: {
            break;
        }
    }

    HEngine::instance().start();

    for (auto* delPTR : staticTilesBodiesDefs)
        delete delPTR;
/*
    for (auto* body : staticTilesBodies)
        world[0].DestroyBody(body);

    for (auto* body : staticTilesBodies)
        world[1].DestroyBody(body);

    for (auto* body : staticTilesBodies)
        world[2].DestroyBody(body);
*/
    for (auto* delPTR : staticTilesPolygonShape)
        delete delPTR;

    for (auto* delPTR : staticTilesFixturesDefs)
        delete delPTR;

    free(finishText);

    return 0;
}

void button1() {
    gameState = PLAY;
}

void button2() {
    HEngine::instance().exit();
}

void start() {

    mainMenuSceneID = HEngine::instance().addSceneMng("mainMenuScene");

    level1 = HEngine::instance().addSceneMng("level1");
    level2 = HEngine::instance().addSceneMng("level2");
    level3 = HEngine::instance().addSceneMng("level3");

    sceneID = level1;
    createLevel1();
    sceneID = level2;
    createLevel2();
    sceneID = level3;
    createLevel3();

    pauseMenu = HEngine::instance().addUIMng("pauseMenu");
    mainMenuUI = HEngine::instance().addUIMng("mainMenuUI");
    gameOver = HEngine::instance().addUIMng("gameOver");
    victory = HEngine::instance().addUIMng("victory");

    createPauseMenu();

    sceneID = mainMenuSceneID;
    createMainMenu();

    createGameOver();
    createVictory();

    EventHandler<KeyboardKeyEvent> keyCall = keyCallback;
    HEevent::Subscribe<KeyboardKeyEvent>(keyCall);

}

void update() {
    CURRENT_SCENE

    switch (gameState) {
        case MAIN_MENU: {
            HEngine::instance().bindScnMng(mainMenuSceneID);
            HEngine::instance().bindUIMng(mainMenuUI);

            break;
        }

        case PLAY: {
            fireMove();

            HEngine::instance().bindScnMng(sceneID);

            shaderUnif timeUnif;
            timeUnif.unifName = "time";
            timeUnif.type = UF_FLOAT;
            timeUnif.f1 = (float)hEng::getTime();

            currentScene->setShaderUniform(fireShID, timeUnif);
            currentScene->setShaderUniform(waterShID, timeUnif);

            world[currentLevel].Step(timeStep, velocityIterations, positionIterations);

            HEngine::instance().bindUIMng(0);

            auto& cam = currentScene->getCameraByID(camID);
            cam->keyboardMove = true;

            b2Vec2 firePos = playerBody[currentLevel]->GetPosition();
            firePos.y -= 6.0f / physSCALE;
            for (auto* it = world[currentLevel].GetBodyList(); it != nullptr ; it = it->GetNext()) {
                for (auto* Fit = it->GetFixtureList(); Fit != nullptr ; Fit = Fit->GetNext()) {
                    if (Fit->TestPoint(firePos)) {
                        if (uintptr_t ptr = it->GetUserData().pointer) {
                            const char* finStr = "finish";
                            const char* loseStr = "lose";

                            if (strcmp((char*)ptr, finStr) == 0)
                                gameState = WIN;

                            if (strcmp((char*)ptr, loseStr) == 0)
                                gameState = GAME_OVER;
                        }
                    }
                }
            }

            break;
        }

        case PAUSE: {
            HEngine::instance().bindUIMng(pauseMenu);

            auto& cam = currentScene->getCameraByID(camID);
            cam->keyboardMove = false;
            break;
        }

        case GAME_OVER: {
            HEngine::instance().bindUIMng(gameOver);

            auto& cam = currentScene->getCameraByID(camID);
            cam->keyboardMove = false;
            break;
        }

        case WIN: {
            HEngine::instance().bindUIMng(victory);

            auto& cam = currentScene->getCameraByID(camID);
            cam->keyboardMove = false;
            break;
        }

        default: {
            break;
        }
    }
}

void keyCallback(const KeyboardKeyEvent& e) {
    CURRENT_SCENE

    if (e.action == HE_RELEASE && e.key == HE_KEY_ESCAPE && (gameState == PLAY || gameState == PAUSE)) {
        gameState = gameState == PLAY? PAUSE : PLAY;
    }
}

void createPauseMenu() {
    auto& currentUI = HEngine::instance().getUIMngByID(pauseMenu);

    unsigned int backRectID = currentUI->addRect(REN_BUF_WIDTH / 2, REN_BUF_HEIGHT / 2, REN_BUF_WIDTH, REN_BUF_HEIGHT);
    unsigned int fireGameTextID = currentUI->addRect(REN_BUF_WIDTH / 2 + 2, REN_BUF_HEIGHT / 2 + 50, 119, 30);

    unsigned int but2 = currentUI->addButton(REN_BUF_WIDTH / 2, REN_BUF_HEIGHT / 2 + 20, 90, 30);
    unsigned int but1 = currentUI->addButton(REN_BUF_WIDTH / 2, REN_BUF_HEIGHT / 2 - 20, 90, 30);

    unsigned int playTextID = currentUI->addRect(REN_BUF_WIDTH / 2 + 2, REN_BUF_HEIGHT / 2 + 20, 130, 39);
    unsigned int exitTextID = currentUI->addRect(REN_BUF_WIDTH / 2 + 2, REN_BUF_HEIGHT / 2 - 20, 130, 39);

    rectShaderID = currentUI->newShader("rectShader");
    auto& rectSh = currentUI->getShaderByID(rectShaderID);

    unsigned int fireGameTextTEXID = currentUI->loadTexture2D("fireGameTextTEX", "../TileMap/FIREGAME.png");

    unsigned int playTextTEXID = currentUI->loadTexture2D("fireGameTextTEX", "../TileMap/PLAY.png");
    unsigned int exitTextTEXID = currentUI->loadTexture2D("fireGameTextTEX", "../TileMap/EXIT.png");

    currentUI->setObjectTexture(fireGameTextID, fireGameTextTEXID);

    currentUI->setObjectTexture(playTextID, playTextTEXID);
    currentUI->setObjectTexture(exitTextID, exitTextTEXID);

    if (rectSh->loadShader(SH_VERTEX_SHADER, "../shaders/RectShader.vert") != SH_OK)
        printf("RECT VERT SH ERROR\n");

    if (rectSh->loadShader(SH_FRAGMENT_SHADER, "../shaders/RectShader.frag") != SH_OK)
        printf("RECT FRAG SH ERROR\n");

    rectSh->genProgram();

    currentUI->getObjectByID(backRectID)->objShaderID = rectShaderID;

    std::function<void()> button1Func(button1);
    std::function<void()> button2Func(button2);

    currentUI->bindOnClickFunc(but2, button1Func);
    currentUI->bindOnClickFunc(but1, button2Func);
}

void createMainMenu() {
    CURRENT_SCENE

    unsigned int mainMenuCamID = currentScene->newCamera("mainCam", 200.0f);
    auto& cam = currentScene->getCameraByID(mainMenuCamID);

    cam->keyboardMove = false;
    cam->setCameraPos(glm::vec3(-64.0f, 64.0f, 0.0f));

    currentScene->bindCamera(mainMenuCamID);

    currentScene->setBackGroundImage("backgroundImage", "../TileMap/background0.png");

    auto& currentUI = HEngine::instance().getUIMngByID(mainMenuUI);

    unsigned int backRectID = currentUI->addRect(REN_BUF_WIDTH / 2, REN_BUF_HEIGHT / 2, REN_BUF_WIDTH, REN_BUF_HEIGHT);
    unsigned int fireGameTextID = currentUI->addRect(REN_BUF_WIDTH / 2 + 2, REN_BUF_HEIGHT / 2 + 50, 119, 30);

    unsigned int but2 = currentUI->addButton(REN_BUF_WIDTH / 2, REN_BUF_HEIGHT / 2 + 20, 90, 30);
    unsigned int but1 = currentUI->addButton(REN_BUF_WIDTH / 2, REN_BUF_HEIGHT / 2 - 20, 90, 30);
    unsigned int but3 = currentUI->addButton(REN_BUF_WIDTH / 2, REN_BUF_HEIGHT / 2 - 60, 90, 30);
    unsigned int but4 = currentUI->addButton(REN_BUF_WIDTH / 2, REN_BUF_HEIGHT / 2 - 100, 90, 30);

    unsigned int level1TextID = currentUI->addRect(REN_BUF_WIDTH / 2 + 2, REN_BUF_HEIGHT / 2 + 20, 100, 39);
    unsigned int level2TextID = currentUI->addRect(REN_BUF_WIDTH / 2 + 2, REN_BUF_HEIGHT / 2 - 20, 100, 39);
    unsigned int level3TextID = currentUI->addRect(REN_BUF_WIDTH / 2 + 2, REN_BUF_HEIGHT / 2 - 60, 100, 39);

    unsigned int exitTextID = currentUI->addRect(REN_BUF_WIDTH / 2 + 2, REN_BUF_HEIGHT / 2 - 100, 130, 39);

    rectShaderID = currentUI->newShader("rectShader");
    auto& rectSh = currentUI->getShaderByID(rectShaderID);

    unsigned int fireGameTextTEXID = currentUI->loadTexture2D("fireGameTextTEX", "../TileMap/FIREGAME.png");

    unsigned int level1TEXID = currentUI->loadTexture2D("fireGameTextTEX", "../TileMap/LEVEL1.png");
    unsigned int level2TEXID = currentUI->loadTexture2D("fireGameTextTEX", "../TileMap/LEVEL2.png");
    unsigned int level3TEXID = currentUI->loadTexture2D("fireGameTextTEX", "../TileMap/LEVEL3.png");

    unsigned int exitTextTEXID = currentUI->loadTexture2D("fireGameTextTEX", "../TileMap/EXIT.png");

    currentUI->setObjectTexture(fireGameTextID, fireGameTextTEXID);

    currentUI->setObjectTexture(level1TextID, level1TEXID);
    currentUI->setObjectTexture(level2TextID, level2TEXID);
    currentUI->setObjectTexture(level3TextID, level3TEXID);

    currentUI->setObjectTexture(exitTextID, exitTextTEXID);

    if (rectSh->loadShader(SH_VERTEX_SHADER, "../shaders/RectShader.vert") != SH_OK)
        printf("RECT VERT SH ERROR\n");

    if (rectSh->loadShader(SH_FRAGMENT_SHADER, "../shaders/RectShader.frag") != SH_OK)
        printf("RECT FRAG SH ERROR\n");

    rectSh->genProgram();

    currentUI->getObjectByID(backRectID)->objShaderID = rectShaderID;

    std::function<void()> level1But([=](){
        gameState = PLAY;
        sceneID = level1;
        currentLevel = 0;
    });

    std::function<void()> level2But([=](){
        gameState = PLAY;
        sceneID = level2;
        currentLevel = 1;
    });

    std::function<void()> level3But([=](){
        gameState = PLAY;
        sceneID = level3;
        currentLevel = 2;
    });

    std::function<void()> button2Func(button2);

    currentUI->bindOnClickFunc(but2, level1But);
    currentUI->bindOnClickFunc(but1, level2But);
    currentUI->bindOnClickFunc(but3, level3But);
    currentUI->bindOnClickFunc(but4, button2Func);
}

void createGameOver() {
    auto& currentUI = HEngine::instance().getUIMngByID(gameOver);

    unsigned int backRectID = currentUI->addRect(REN_BUF_WIDTH / 2, REN_BUF_HEIGHT / 2, REN_BUF_WIDTH, REN_BUF_HEIGHT);
    unsigned int gameOverTextID = currentUI->addRect(REN_BUF_WIDTH / 2 + 5, REN_BUF_HEIGHT / 2 + 5, 400, 200);

    rectShaderID = currentUI->newShader("rectGameOverShader");
    auto& rectSh = currentUI->getShaderByID(rectShaderID);

    unsigned int gameOverTextTEXID = currentUI->loadTexture2D("gameOverTextTEX", "../TileMap/GAMEOVER.png");

    currentUI->setObjectTexture(gameOverTextID, gameOverTextTEXID);

    if (rectSh->loadShader(SH_VERTEX_SHADER, "../shaders/RectShader.vert") != SH_OK)
        printf("RECT VERT SH ERROR\n");

    if (rectSh->loadShader(SH_FRAGMENT_SHADER, "../shaders/RectShader.frag") != SH_OK)
        printf("RECT FRAG SH ERROR\n");

    rectSh->genProgram();

    currentUI->getObjectByID(backRectID)->objShaderID = rectShaderID;
}

void createVictory() {
    auto& currentUI = HEngine::instance().getUIMngByID(victory);

    unsigned int backRectID = currentUI->addRect(REN_BUF_WIDTH / 2, REN_BUF_HEIGHT / 2, REN_BUF_WIDTH, REN_BUF_HEIGHT);
    unsigned int victoryTextID = currentUI->addRect(REN_BUF_WIDTH / 2 + 5, REN_BUF_HEIGHT / 2 + 5, 400, 200);

    rectShaderID = currentUI->newShader("rectVictoryShader");
    auto& rectSh = currentUI->getShaderByID(rectShaderID);

    unsigned int victoryTextTEXID = currentUI->loadTexture2D("victoryTextTEX", "../TileMap/VICTORY.png");

    currentUI->setObjectTexture(victoryTextID, victoryTextTEXID);

    if (rectSh->loadShader(SH_VERTEX_SHADER, "../shaders/RectShader.vert") != SH_OK)
        printf("RECT VERT SH ERROR\n");

    if (rectSh->loadShader(SH_FRAGMENT_SHADER, "../shaders/RectShader.frag") != SH_OK)
        printf("RECT FRAG SH ERROR\n");

    rectSh->genProgram();

    currentUI->getObjectByID(backRectID)->objShaderID = rectShaderID;
}

void fireMove() {
    CURRENT_SCENE

    b2Vec2 fireVel = playerBody[currentLevel]->GetLinearVelocity();

    if (hEng::getKeyStatus(HE_KEY_RIGHT))
        if (fireVel.x < 5.0f)
            playerBody[currentLevel]->ApplyForceToCenter(b2Vec2(5.f, 0.0f), true);

    if (hEng::getKeyStatus(HE_KEY_LEFT))
        if (fireVel.x > -5.0f)
            playerBody[currentLevel]->ApplyForceToCenter(b2Vec2(-5.f, 0.0f), true);

    if (hEng::getKeyStatus(HE_KEY_SPACE)) {

        b2Vec2 firePos = playerBody[currentLevel]->GetPosition();
        firePos.y -= 6.0f / physSCALE;

        bool fireOnGround = false;

        for (auto* it = world[currentLevel].GetBodyList(); it != nullptr ; it = it->GetNext()) {
            for (auto* Fit = it->GetFixtureList(); Fit != nullptr ; Fit = Fit->GetNext()) {
                if (Fit->TestPoint(firePos))
                    fireOnGround = true;
            }
        }

        if (fireOnGround)
            playerBody[currentLevel]->ApplyForceToCenter(b2Vec2(0.0f, 100.0f), true);
    }

    b2Vec2 pos = playerBody[currentLevel]->GetPosition();
    pos *= physSCALE;
    pos += b2Vec2(0.0f, 9.0f);

    currentScene->getObjectByID(fire)->position = glm::vec3(pos.x, pos.y, 0.1f);
}

void createLevel1() {
    CURRENT_SCENE

    camID = currentScene->newCamera("mainCam", 200.0f);
    auto& cam = currentScene->getCameraByID(camID);

    cam->keyboardMove = true;
    cam->setKeysForMove(HE_KEY_W, HE_KEY_S, HE_KEY_A, HE_KEY_D);
    cam->setCameraPos(glm::vec3(-64.0f, -64.0f, 0.0f));

    currentScene->bindCamera(camID);

    tileMapTexID = currentScene->loadTexture2D("TileMap", "../TileMap/15BlobTileMap16x16.png");
    HETexture tileMapTex = currentScene->getTextureByID(tileMapTexID);
    tileMapSize.tileMapWidth = tileMapTex.width;
    tileMapSize.tileMapHeight = tileMapTex.height;

    currentScene->setBackGroundImage("backgroundImage", "../TileMap/background0.png");

    currentLevel = 0;
    createPlayer(16.0f, 30.0f);
    createFinish(8.5f, 2.5f);

    createStaticTile(0, 0, 1.0f, 1.0f);
    createStaticTile(1, 0, 2.0f, 1.0f);
    createStaticTile(1, 0, 3.0f, 1.0f);

    createTile(6, 7, 4.0f, 1.0f);
    createTile(1, 7, 5.0f, 1.0f);
    createTile(5, 7, 6.0f, 1.0f);

    createStaticTile(0, 1, 1.0f, 0.0f);
    createStaticTile(1, 1, 2.0f, 0.0f);
    createStaticTile(1, 1, 3.0f, 0.0f);

    createStaticTile(0, 2, 1.0f, -1.0f);
    createStaticTile(1, 2, 2.0f, -1.0f);
    createStaticTile(1, 2, 3.0f, -1.0f);
    createStaticTile(1, 2, 4.0f, -1.0f);
    createStaticTile(1, 2, 5.0f, -1.0f);
    createStaticTile(1, 2, 6.0f, -1.0f);
    createStaticTile(1, 2, 7.0f, -1.0f);
    createStaticTile(1, 2, 8.0f, -1.0f);

    createStaticTile(1, 0, 7.0f, 1.0f);
    createStaticTile(1, 0, 8.0f, 1.0f);
    createStaticTile(2, 0, 9.0f, 1.0f);

    createStaticTile(1, 1, 7.0f, 0.0f);
    createStaticTile(1, 1, 8.0f, 0.0f);
    createStaticTile(2, 1, 9.0f, 0.0f);
    createStaticTile(2, 2, 9.0f, -1.0f);

    createWater(80.0f, -8.0f, 48.0f, 26.0f);

}

void createLevel2() {
    CURRENT_SCENE

    camID = currentScene->newCamera("mainCam", 200.0f);
    auto& cam = currentScene->getCameraByID(camID);

    cam->keyboardMove = true;
    cam->setKeysForMove(HE_KEY_W, HE_KEY_S, HE_KEY_A, HE_KEY_D);
    cam->setCameraPos(glm::vec3(-64.0f, -64.0f, 0.0f));

    currentScene->bindCamera(camID);

    tileMapTexID = currentScene->loadTexture2D("TileMap", "../TileMap/15BlobTileMap16x16.png");
    HETexture tileMapTex = currentScene->getTextureByID(tileMapTexID);
    tileMapSize.tileMapWidth = tileMapTex.width;
    tileMapSize.tileMapHeight = tileMapTex.height;

    currentScene->setBackGroundImage("backgroundImage", "../TileMap/background0.png");

    currentLevel = 1;
    createPlayer(16.0f, 30.0f);
    createFinish(15.5f, 2.5f);

    createStaticTile(0, 0, 1.0f, 0.0f);
    createStaticTile(2, 0, 2.0f, 0.0f);
    createStaticTile(0, 2, 1.0f, -1.0f);
    createStaticTile(2, 2, 2.0f, -1.0f);

    createStaticTile(3, 3, 6.0f, 3.0f);
    createStaticTile(3, 3, 10.0f, 1.0f);

    createStaticTile(0, 0, 15.0f, 1.0f);
    createStaticTile(2, 0, 16.0f, 1.0f);
    createStaticTile(0, 2, 15.0f, 0.0f);
    createStaticTile(2, 2, 16.0f, 0.0f);
}

void createLevel3() {
    CURRENT_SCENE

    camID = currentScene->newCamera("mainCam", 200.0f);
    auto& cam = currentScene->getCameraByID(camID);

    cam->keyboardMove = true;
    cam->setKeysForMove(HE_KEY_W, HE_KEY_S, HE_KEY_A, HE_KEY_D);
    cam->setCameraPos(glm::vec3(-64.0f, -64.0f, 0.0f));

    currentScene->bindCamera(camID);

    tileMapTexID = currentScene->loadTexture2D("TileMap", "../TileMap/15BlobTileMap16x16.png");
    HETexture tileMapTex = currentScene->getTextureByID(tileMapTexID);
    tileMapSize.tileMapWidth = tileMapTex.width;
    tileMapSize.tileMapHeight = tileMapTex.height;

    currentScene->setBackGroundImage("backgroundImage", "../TileMap/background0.png");

    currentLevel = 2;

    createPlayer(16.0f, 30.0f);
    createFinish(19.5f, 2.5f);

    createStaticTile(0, 0, 1.0f, 0.0f);
    createStaticTile(2, 0, 2.0f, 0.0f);
    createStaticTile(0, 2, 1.0f, -1.0f);
    createStaticTile(2, 2, 2.0f, -1.0f);


    float offsetX = 7.0f;
    float offsetY = 0.0f;


    createStaticTile(0, 0, 1.0f + offsetX, 1.0f + offsetY);
    createStaticTile(0, 1, 1.0f + offsetX, 0.0f + offsetY);
    createStaticTile(0, 2, 1.0f + offsetX, -1.0f + offsetY);
    createStaticTile(1, 2, 2.0f + offsetX, -1.0f + offsetY);
    createStaticTile(1, 2, 3.0f + offsetX, -1.0f + offsetY);
    createStaticTile(1, 2, 4.0f + offsetX, -1.0f + offsetY);
    createStaticTile(1, 2, 5.0f + offsetX, -1.0f + offsetY);
    createStaticTile(1, 2, 6.0f + offsetX, -1.0f + offsetY);
    createStaticTile(1, 2, 7.0f + offsetX, -1.0f + offsetY);
    createStaticTile(2, 2, 8.0f + offsetX, -1.0f + offsetY);
    createStaticTile(2, 1, 8.0f + offsetX, 0.0f + offsetY);
    createStaticTile(2, 0, 8.0f + offsetX, 1.0f + offsetY);

    createStaticTile(0, 0, 19.0f, 1.0f);
    createStaticTile(2, 0, 20.0f, 1.0f);
    createStaticTile(0, 2, 19.0f, 0.0f);
    createStaticTile(2, 2, 20.0f, 0.0f);

    createWater(184.0f, 10.0f, 96.0f, 36.0f);
}

void createPlayer(float posX, float posY) {
    CURRENT_SCENE

    fire = currentScene->createEmptyObject("fire");

    fireShID = currentScene->newShader("fireSh");

    auto& fireSH = currentScene->getShaderByID(fireShID);

    if (fireSH->loadShader(SH_VERTEX_SHADER, "../shaders/pnt.vert") != SH_OK)
        printf("FIRE VERT SH ERROR\n");
    if (fireSH->loadShader(SH_FRAGMENT_SHADER, "../shaders/pnt.frag") != SH_OK)
        printf("FIRE FRAG SH ERROR\n");

    fireSH->genProgram();

    currentScene->getObjectByID(fire)->objShaderID = fireShID;
    currentScene->getObjectByID(fire)->scale = glm::vec3(32.0f, 32.0f, 0.0f);
    currentScene->getObjectByID(fire)->position.z = 0.1f;

    currentScene->getObjectByID(fire)->alphaChUse = true;

    playerBodyDef[currentLevel].type = b2_dynamicBody;
    playerBodyDef[currentLevel].position.Set(posX / physSCALE, posY / physSCALE);

    playerBody[currentLevel] = world[currentLevel].CreateBody(&playerBodyDef[currentLevel]);
    playerBody[currentLevel]->SetFixedRotation(true);

    playerShape[currentLevel].m_radius = 5.0f/physSCALE;

    playerFix[currentLevel].shape = &playerShape[currentLevel];
    playerFix[currentLevel].density = 6.0f;
    playerFix[currentLevel].friction = 0.3f;

    playerBody[currentLevel]->CreateFixture(&playerFix[currentLevel]);
}

void createFinish(float posX, float posY) {
    CURRENT_SCENE

    finishID = currentScene->createEmptyObject("fire");

    currentScene->getObjectByID(finishID)->scale = glm::vec3(32.0f, 32.0f, 0.0f);
    currentScene->getObjectByID(finishID)->position = glm::vec3((16.0f * posX), (16.0f * posY), 0.99f);
    currentScene->getObjectByID(finishID)->alphaChUse = true;

    finishTEXID = currentScene->loadTexture2D("finishTEX", "../TileMap/torch.png");
    currentScene->setObjectTexture(finishID, finishTEXID);

    auto* newTileBodyDef = new b2BodyDef;

    newTileBodyDef->type = b2_staticBody;
    newTileBodyDef->position.Set((16.0f * posX) / physSCALE, (16.0f * (posY - 0.5f)) / physSCALE);

    finishText = malloc(sizeof(char) * 8);

    for (int i = 0; i < 7; ++i)
        ((char*)finishText)[i] = "finish"[i];

    ((char*)finishText)[7] = '\0';

    b2BodyUserData data_U;
    data_U.pointer = (uintptr_t)finishText;

    newTileBodyDef->userData = data_U;

    b2Body* newTileBody = world[currentLevel].CreateBody(newTileBodyDef);

    auto* newTileShape = new b2PolygonShape;
    newTileShape->SetAsBox(2.0f / physSCALE,8.0f / physSCALE);

    auto* newTileFixtureDef = new b2FixtureDef;
    newTileFixtureDef->shape = newTileShape;
    newTileFixtureDef->friction = 0.3f;

    newTileBody->CreateFixture(newTileFixtureDef);

    staticTilesBodiesDefs.push_back(newTileBodyDef);
    staticTilesBodies.push_back(newTileBody);
    staticTilesPolygonShape.push_back(newTileShape);
    staticTilesFixturesDefs.push_back(newTileFixtureDef);
}

void createStaticTile(int tileIDX, int tileIDY, float tileCoordX, float tileCoordY) {

    float tilePosX = tileCoordX * 16.0f;
    float tilePosY = tileCoordY * 16.0f;

    createTile(tileIDX, tileIDY, tileCoordX, tileCoordY);

    auto* newTileBodyDef = new b2BodyDef;

    newTileBodyDef->type = b2_staticBody;
    newTileBodyDef->position.Set(tilePosX / physSCALE, tilePosY / physSCALE);

    b2Body* newTileBody = world[currentLevel].CreateBody(newTileBodyDef);

    auto* newTileShape = new b2PolygonShape;
    newTileShape->SetAsBox(8.0f / physSCALE,8.0f / physSCALE);

    auto* newTileFixtureDef = new b2FixtureDef;
    newTileFixtureDef->shape = newTileShape;
    newTileFixtureDef->friction = 0.3f;

    newTileBody->CreateFixture(newTileFixtureDef);

    staticTilesBodiesDefs.push_back(newTileBodyDef);
    staticTilesBodies.push_back(newTileBody);
    staticTilesPolygonShape.push_back(newTileShape);
    staticTilesFixturesDefs.push_back(newTileFixtureDef);
}

void createTile(int tileIDX, int tileIDY, float tileCoordX, float tileCoordY) {
    CURRENT_SCENE

    static std::string tilePrefix = "tileNUM_";

    float tilePosX = tileCoordX * 16.0f;
    float tilePosY = tileCoordY * 16.0f;

    std::string tileName = tilePrefix + std::to_string(tilesCount);
    unsigned int newTileID = currentScene->createTile(tileName, tileMapSize, tileIDX, tileIDY);

    currentScene->setObjectTexture(newTileID, tileMapTexID);
    currentScene->getObjectByID(newTileID)->position = glm::vec3(tilePosX, tilePosY, -0.05f);
    tilesCount++;
}

void createWater(float posX, float posY, float w, float h) {
    CURRENT_SCENE

    unsigned int water = currentScene->createEmptyObject("water");

    currentScene->getObjectByID(water)->position = glm::vec3(posX, posY, 0.2f);
    currentScene->getObjectByID(water)->scale = glm::vec3(w, h, 0.0f);
    currentScene->getObjectByID(water)->alphaChUse = true;

    waterShID = currentScene->newShader("waterSh");
    auto& waterSh = currentScene->getShaderByID(waterShID);

    if (waterSh->loadShader(SH_VERTEX_SHADER, "../shaders/water.vert") != SH_OK)
        printf("WATER VERT SH ERROR\n");

    if (waterSh->loadShader(SH_FRAGMENT_SHADER, "../shaders/water.frag") != SH_OK)
        printf("WATER FRAG SH ERROR\n");

    waterSh->genProgram();

    currentScene->getObjectByID(water)->objShaderID = waterShID;

    auto* newTileBodyDef = new b2BodyDef;

    newTileBodyDef->type = b2_staticBody;
    newTileBodyDef->position.Set(posX / physSCALE, posY / physSCALE);

    finishText = malloc(sizeof(char) * 5);

    for (int i = 0; i < 4; ++i)
        ((char*)finishText)[i] = "lose"[i];

    ((char*)finishText)[4] = '\0';

    b2BodyUserData data_U;
    data_U.pointer = (uintptr_t)finishText;

    newTileBodyDef->userData = data_U;

    b2Body* newTileBody = world[currentLevel].CreateBody(newTileBodyDef);

    auto* newTileShape = new b2PolygonShape;
    newTileShape->SetAsBox((w / 2.0f) / physSCALE,1.0f / physSCALE);

    auto* newTileFixtureDef = new b2FixtureDef;
    newTileFixtureDef->shape = newTileShape;
    newTileFixtureDef->friction = 0.3f;

    newTileBody->CreateFixture(newTileFixtureDef);

    staticTilesBodiesDefs.push_back(newTileBodyDef);
    staticTilesBodies.push_back(newTileBody);
    staticTilesPolygonShape.push_back(newTileShape);
    staticTilesFixturesDefs.push_back(newTileFixtureDef);
}
