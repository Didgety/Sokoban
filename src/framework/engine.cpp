#include "engine.h"
#include <fstream>
#include <random>

enum state {menu, levelSelect, instructions, pause, play, levelComplete};
state screen;

using std::to_string;

// Colors
color button, buttonComplete, buttonHover, completeHover, buttonClick, shadow,
        wallColor, walkableColor, boxColor, boxOnTarget, targetColor, playerColor;

Engine::Engine() : keys() {
    this->initWindow();
    this->initShaders();
    this->initShapes();

    wallColor = {0.5, 0.5, 0.5, 1};  // grey
    walkableColor = {1, 1, 1, 1};    // white
    boxColor = {0.5, 0.25, 0, 1};    // brown
    targetColor = {1, 0.5, 0, 1};    // orange
    playerColor = {1, 0, 1, 1};      // purple
    button = {1, 0, 0, 1};           // red
    buttonComplete = {0, 0.75, 0, 1};   // green for complete levels
    shadow = {0.5, 0.5, 0.5, 0};
    buttonHover.vec = button.vec + shadow.vec; // light version of button
    completeHover.vec = buttonComplete.vec + shadow.vec;
    buttonClick.vec = button.vec - shadow.vec; // dark version of button
    boxOnTarget.vec = (boxColor.vec + shadow.vec); // light brownish/orange
}

Engine::~Engine() {}

unsigned int Engine::initWindow(bool debug) {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    window = glfwCreateWindow(width, height, "engine", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // OpenGL configuration
    glViewport(0, 0, width, height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSwapInterval(1);

    // activate event listener
    glfwSetKeyCallback(window, EngineState::keyCallbackDispatch);

    return 0;
}

void Engine::initShaders() {
    // load shader manager
    shaderManager = make_unique<ShaderManager>();

    // Load shader into shader manager and retrieve it
    shapeShader = this->shaderManager->loadShader("../res/shaders/shape.vert", "../res/shaders/shape.frag",  nullptr, "shape");

    // Configure text shader and renderer
    textShader = shaderManager->loadShader("../res/shaders/text.vert", "../res/shaders/text.frag", nullptr, "text");
    fontRenderer = make_unique<FontRenderer>(shaderManager->getShader("text"), "../res/fonts/MxPlus_IBM_BIOS.ttf", 24);

    // Set uniforms
    textShader.use().setVector2f("vertex", vec4(100, 100, .5, .5));
    shapeShader.use().setMatrix4("projection", this->PROJECTION);
}

void Engine::initShapes() {
    // menu screen
    startButton = make_unique<Rect>(shapeShader,
                                    vec2{width/2,height/2 + 75},
                                    vec2{100, 50},
                                    color{1, 0, 0, 1});
    startButtonShadow = make_unique<Rect>(shapeShader,
                                          vec2{width/2 + 5,height/2 + 70},
                                          vec2{100, 50},
                                          color{0.5, 0, 0, 1});
    levelSelectButton = make_unique<Rect>(shapeShader,
                                          vec2{width/2,height/2},
                                          vec2{100, 50},
                                          color{1, 0, 0, 1});
    levelSelectButtonShadow = make_unique<Rect>(shapeShader,
                                                vec2{width/2 + 5,height/2 - 5},
                                                vec2{100, 50},
                                                color{0.5, 0, 0, 1});
    quitButton = make_unique<Rect>(shapeShader,
                                   vec2{width/2,height/2 - 75},
                                   vec2{100, 50},
                                   color{1, 0, 0, 1});
    quitButtonShadow = make_unique<Rect>(shapeShader,
                                         vec2{width/2 + 5,height/2 - 80},
                                         vec2{100, 50},
                                         color{0.5, 0, 0, 1});
    // instructions menu
    continueButton = make_unique<Rect>(shapeShader,
                                       vec2{width/2,height/2 - 150},
                                       vec2{100, 50},
                                       color{1, 0, 0, 1});
    continueButtonShadow = make_unique<Rect>(shapeShader,
                                             vec2{width/2 + 5,height/2 - 155},
                                             vec2{100, 50},
                                             color{0.5, 0, 0, 1});
    // levelSelect screen
    for(int i {0}; i < MAX_LEVEL; ++i) {
        levelSelectButtons.push_back(make_unique<Rect>(shapeShader,
                                                       vec2{width/2 - ((i - 2) * 75),height/2},
                                                       vec2{50, 50},
                                                       color{1, 0, 0, 1}));
        levelSelectButtonsShadows.push_back(make_unique<Rect>(shapeShader,
                                                              vec2{width/2 - ((i - 2) * 75) + 5 ,height/2 - 5},
                                                              vec2{50, 50},
                                                              color{0.5, 0, 0, 1}));
    }
    levelMenuButton = make_unique<Rect>(shapeShader,
                                   vec2{width/2,height/2 - 100},
                                   vec2{100, 50},
                                   color{1, 0, 0, 1});
    levelMenuButtonShadow = make_unique<Rect>(shapeShader,
                                         vec2{width/2 + 5,height/2 - 105},
                                         vec2{100, 50},
                                         color{0.5, 0, 0, 1});
    // levelComplete screen
    nextLevelButton = make_unique<Rect>(shapeShader,
                                       vec2{width/2 - 125,height/2 - 100},
                                       vec2{100, 50},
                                       color{1, 0, 0, 1});
    nextLevelButtonShadow = make_unique<Rect>(shapeShader,
                                             vec2{width/2 - 120,height/2 - 105},
                                             vec2{100, 50},
                                             color{0.5, 0, 0, 1});
    completeMenuButton = make_unique<Rect>(shapeShader,
                                        vec2{width/2,height/2 - 100},
                                        vec2{100, 50},
                                        color{1, 0, 0, 1});
    completeMenuButtonShadow = make_unique<Rect>(shapeShader,
                                              vec2{width/2 + 5,height/2 - 105},
                                              vec2{100, 50},
                                              color{0.5, 0, 0, 1});
    completeLevelButton = make_unique<Rect>(shapeShader,
                                        vec2{width/2 + 125,height/2 - 100},
                                        vec2{100, 50},
                                        color{1, 0, 0, 1});
    completeLevelButtonShadow = make_unique<Rect>(shapeShader,
                                              vec2{width/2 + 130,height/2 - 105},
                                              vec2{100, 50},
                                              color{0.5, 0, 0, 1});
    // pause screen
    resumeButton = make_unique<Rect>(shapeShader,
                                     vec2{width/2,height/2 + 75},
                                     vec2{100, 50},
                                     color{1, 0, 0, 1});
    resumeButtonShadow = make_unique<Rect>(shapeShader,
                                           vec2{width/2 + 5,height/2 + 70},
                                           vec2{100, 50},
                                           color{0.5, 0, 0, 1});
    menuButton = make_unique<Rect>(shapeShader,
                                   vec2{width/2,height/2},
                                   vec2{100, 50},
                                   color{1, 0, 0, 1});
    menuButtonShadow = make_unique<Rect>(shapeShader,
                                         vec2{width/2 + 5,height/2 - 5},
                                         vec2{100, 50},
                                         color{0.5, 0, 0, 1});
    restartButton = make_unique<Rect>(shapeShader,
                                   vec2{width/2,height/2 - 75},
                                   vec2{100, 50},
                                   color{1, 0, 0, 1});
    restartButtonShadow = make_unique<Rect>(shapeShader,
                                         vec2{width/2 + 5,height/2 - 80},
                                         vec2{100, 50},
                                         color{0.5, 0, 0, 1});
    // instructions screen
    player = make_unique<Rect>(shapeShader,
                               vec2{width/2,height/2 + 100},
                               vec2{50, 50},
                               color{1, 0, 1, 1});
    wall = make_unique<Rect>(shapeShader,
                             vec2{width/2 - 150,height/2},
                             vec2{50, 50},
                             color{0.5, 0.5, 0.5, 1});
    walkable = make_unique<Rect>(shapeShader,
                                 vec2{width/2 - 50,height/2},
                                 vec2{50, 50},
                                 color{1, 1, 1, 1});
    box = make_unique<Rect>(shapeShader,
                            vec2{width/2 + 50,height/2},
                            vec2{50, 50},
                            color{0.5, 0.25, 0, 1});
    target = make_unique<Rect>(shapeShader,
                               vec2{width/2 + 150,height/2},
                               vec2{50, 50},
                               color{1, 0.5, 0, 1});
}

void Engine::processInput() {
    glfwPollEvents();

    // Set keys to true if pressed, false if released
    for (int key = 0; key < 1024; ++key) {
        if (glfwGetKey(window, key) == GLFW_PRESS)
            keys[key] = true;
        else if (glfwGetKey(window, key) == GLFW_RELEASE)
            keys[key] = false;
    }

    // Mouse position saved to check for collisions
    glfwGetCursorPos(window, &MouseX, &MouseY);
    // Mouse position is inverted because the origin of the window is in the top left corner
    MouseY = height - MouseY; // Invert y-axis of mouse position
    bool mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    switch(screen) {
        case menu: {
            if(startButton->isOverlapping(vec2(MouseX, MouseY))) {
                startButton->setColor(buttonHover);
                if(mousePressed) { startButton ->setColor(buttonClick); }
                if(!mousePressed && mousePressedLastFrame) {
                    screen = instructions;
                }
            } else{
                startButton->setColor(button);
            }

            if(levelSelectButton->isOverlapping(vec2(MouseX, MouseY))) {
                levelSelectButton ->setColor(buttonHover);
                if(mousePressed) { levelSelectButton ->setColor(buttonClick); }
                if(!mousePressed && mousePressedLastFrame) {
                    screen = levelSelect;
                }
            } else {
                levelSelectButton->setColor(button);
            }

            if(quitButton->isOverlapping(vec2(MouseX, MouseY))) {
                quitButton ->setColor(buttonHover);
                if(mousePressed) { quitButton ->setColor(buttonClick); }
                if(!mousePressed && mousePressedLastFrame) {
                    glfwSetWindowShouldClose(window, true);
                }
            } else {
                quitButton->setColor(button);
            }
            break;
        }
        case instructions: {
            // shows instructions and continue button to move on to the first level
            if(continueButton->isOverlapping(vec2(MouseX, MouseY))) {
                continueButton->setColor(buttonHover);
                if(mousePressed) { continueButton->setColor(buttonClick); }
                if(!mousePressed && mousePressedLastFrame) {
                    currLevel = 1;
                    initLevel(currLevel); // always start at level 1 when using the start button
                    completedLevels[0] = false; // set to false in case player has already beat the level
                    startTime = (float)glfwGetTime();
                    moves = 0;
                    screen = play;
                }
            } else {
                continueButton->setColor(button);
            }
            break;
        }
        case levelSelect: {
            // each button takes you to a corresponding level.
            // colored red if you have not beat the level,
            // green if you have beat the level
            for(int i {0}; i < MAX_LEVEL; ++i) {
                if(levelSelectButtons[i]->isOverlapping(vec2(MouseX, MouseY))) {
                    if(completedLevels[i]) {
                        levelSelectButtons[i]->setColor(completeHover);
                    } else {
                        levelSelectButtons[i]->setColor(buttonHover);
                    }
                    if(mousePressed && !completedLevels[i]) {
                        levelSelectButtons[i]->setColor(buttonClick);
                    } else if(mousePressed && completedLevels[i]) {
                        levelSelectButtons[i]->setColor(buttonComplete.vec - shadow.vec);
                    }
                    if(!mousePressed && mousePressedLastFrame) {
                        // if the player clicked a level button, initialize the level, update current level,
                        // start the timer, and switch to the play screen
                        initLevel(5 - i);
                        currLevel = 5 - i;
                        startTime = (float)glfwGetTime();
                        moves = 0;
                        screen = play;
                    }
                } else {
                    if(completedLevels[i]) {
                        levelSelectButtons[i]->setColor(buttonComplete);
                        levelSelectButtonsShadows[i]->setColor(buttonComplete.vec - shadow.vec);
                    } else {
                        levelSelectButtons[i]->setColor(button);
                    }
                }
            }

            if(levelMenuButton->isOverlapping(vec2(MouseX, MouseY))) {
                levelMenuButton->setColor(buttonHover);
                if(mousePressed) { levelMenuButton->setColor(buttonClick); }
                if(!mousePressed && mousePressedLastFrame) {
                    screen = menu;
                }
            } else {
                levelMenuButton->setColor(button);
            }
            break;
        }
        case play: {
            // keyboard input handled by keycallback() and EngineState
            break;
        }
        case pause: {
            // resume level
            if(resumeButton->isOverlapping(vec2(MouseX, MouseY))) {
                resumeButton->setColor(buttonHover);
                if(mousePressed) { startButton->setColor(buttonClick); }
                if(!mousePressed && mousePressedLastFrame) {
                    screen = play;
                }
            } else{
                resumeButton->setColor(button);
            }
            // main menu
            if(menuButton->isOverlapping(vec2(MouseX, MouseY))) {
                menuButton->setColor(buttonHover);
                if(mousePressed) { menuButton->setColor(buttonClick); }
                if(!mousePressed && mousePressedLastFrame) {
                    screen = menu;
                }
            } else{
                menuButton->setColor(button);
            }
            // restart level
            if(restartButton->isOverlapping(vec2(MouseX, MouseY))) {
                restartButton->setColor(buttonHover);
                if(mousePressed) { restartButton->setColor(buttonClick); }
                if(!mousePressed && mousePressedLastFrame) {
                    initLevel(currLevel);
                    moves = 0;
                    startTime = (float)glfwGetTime();
                    screen = play;
                }
            } else{
                restartButton->setColor(button);
            }

            break;
        }
        case levelComplete: {
            if(nextLevelButton->isOverlapping(vec2(MouseX, MouseY))) {
                nextLevelButton->setColor(buttonHover);
                if(mousePressed) { nextLevelButton->setColor(buttonClick); }
                if(!mousePressed && mousePressedLastFrame) {
                    initLevel(currLevel);
                    startTime = (float)glfwGetTime();
                    moves = 0;
                    screen = play;
                }
            } else {
                nextLevelButton->setColor(button);
            }

            if(completeMenuButton->isOverlapping(vec2(MouseX, MouseY))) {
                completeMenuButton->setColor(buttonHover);
                if(mousePressed) { completeMenuButton->setColor(buttonClick); }
                if(!mousePressed && mousePressedLastFrame) {
                    screen = menu;
                }
            } else {
                completeMenuButton->setColor(button);
            }

            if(completeLevelButton->isOverlapping(vec2(MouseX, MouseY))) {
                completeLevelButton->setColor(buttonHover);
                if(mousePressed) { completeLevelButton->setColor(buttonClick); }
                if(!mousePressed && mousePressedLastFrame) {
                    screen = levelSelect;
                }
            } else {
                completeLevelButton->setColor(button);
            }

            break;
        }
    }
    // reset mouse pressed value
    mousePressedLastFrame = mousePressed;
}

void Engine::update() {
    // End the game when all the boxes are in the correct position
    if(finishedLevel) {
        endTime = (float)glfwGetTime();
        deltaTime = endTime - startTime;
        completedLevels[5 - currLevel] = true;
        ++currLevel;
        finishedLevel = false;
        screen = levelComplete;
    }
}

void Engine::render() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color
    glClear(GL_COLOR_BUFFER_BIT);

    // Set shader to use for all shapes
    shapeShader.use();

    // Render differently depending on screen
    switch (screen) {
        case menu: {
            // shadows
            startButtonShadow->setUniforms();
            startButtonShadow->draw();
            levelSelectButtonShadow->setUniforms();
            levelSelectButtonShadow->draw();
            quitButtonShadow->setUniforms();
            quitButtonShadow->draw();
            // buttons
            startButton->setUniforms();
            startButton->draw();
            levelSelectButton->setUniforms();
            levelSelectButton->draw();
            quitButton->setUniforms();
            quitButton->draw();

            fontRenderer->renderText("Sokoban!",
                                     startButton->getPosX() + (12),
                                     startButton->getPosY() + 75,
                                     1,
                                     {1, 1, 1});

            fontRenderer->renderText("Start",
                                     startButton->getPosX() + (12 * 5.7),
                                     startButton->getPosY() - 4,
                                     0.5,
                                     {1, 1, 1});
            fontRenderer->renderText("Levels",
                                     levelSelectButton->getPosX() + (float)(12 * 5.2),
                                     levelSelectButton->getPosY() - 4,
                                     0.5,
                                     {1, 1, 1});
            fontRenderer->renderText("Quit",
                                     quitButton->getPosX() + (float)(12 * 6.2),
                                     quitButton->getPosY() - 4,
                                     0.5,
                                     {1, 1, 1});

            break;
        }
        case instructions: {
            player->setUniforms();
            player->draw();
            wall->setUniforms();
            wall->draw();
            walkable->setUniforms();
            walkable->draw();
            box->setUniforms();
            box->draw();
            target->setUniforms();
            target->draw();
            // shadows
            continueButtonShadow->setUniforms();
            continueButtonShadow->draw();
            // button
            continueButton->setUniforms();
            continueButton->draw();
            // button text
            fontRenderer->renderText("Continue",
                                     continueButton->getPosX() + (float)(12 * 4.2),
                                     continueButton->getPosY() - 4,
                                     0.5, vec3{1, 1, 1});
            // object descriptions
            fontRenderer->renderText("This is you!",
                                     player->getPosX() + (12 * 3), player->getPosY() + 50,
                                     0.5, vec3{1, 1, 1});
            fontRenderer->renderText("Wall",
                                     wall->getPosX() + (float)(12 * 2.3), wall->getPosY() + 35,
                                     0.5, vec3{1, 1, 1});
            fontRenderer->renderText("Floor",
                                     walkable->getPosX() + (float)(12 * 4.5), walkable->getPosY() + 35,
                                     0.5, vec3{1, 1, 1});
            fontRenderer->renderText("Box",
                                     box->getPosX() + (12 * 8), box->getPosY() + 35,
                                     0.5, vec3{1, 1, 1});
            fontRenderer->renderText("Target",
                                     target->getPosX() + (float)(12 * 9.5), target->getPosY() + 35,
                                     0.5, vec3{1, 1, 1});
            // gameplay instructions
            fontRenderer->renderText("Push the boxes onto all the targets",
                                     (float)width - (12 * 34), (float)height/2 - (12 * 6),
                                     0.5, vec3{1, 1, 1});
            fontRenderer->renderText("Move with WASD or the arrow keys",
                                     (float)(width - (12 * 32.25)), (float)height/2 - (12 * 8),
                                     0.5, vec3{1, 1, 1});

            break;
        }
        case levelSelect: {
            for(int i {0}; i < MAX_LEVEL; ++i) {
                // shadows
                levelSelectButtonsShadows[i]->setUniforms();
                levelSelectButtonsShadows[i]->draw();
                // buttons
                levelSelectButtons[i]->setUniforms();
                levelSelectButtons[i]->draw();
            }
            // shadow
            levelMenuButtonShadow->setUniforms();
            levelMenuButtonShadow->draw();
            // button
            levelMenuButton->setUniforms();
            levelMenuButton->draw();

            for(int i {0}; i < MAX_LEVEL; ++i) {
                fontRenderer->renderText(to_string(i + 1),
                                         (float)width/2 + (float)((i - 2) * 100) + 85, (float)height/2 - 6,
                                         1, vec3{1, 1, 1});
            }

            fontRenderer->renderText("Choose a Level",
                                     (float)width/2 - (12 * 5) , (float)height/2 + 75,
                                     1, vec3{1, 1, 1});

            fontRenderer->renderText("Menu",
                                     levelMenuButton->getPosX() + (12 * 5.9), levelMenuButton->getPosY() - 4,
                                     0.5, vec3{1, 1, 1});

            break;
        }
        case play: {
            // Render tiles
            int cols = (int)mapTiles.size();
            int rows = (int)mapTiles[0].size();
            for(int i {0}; i < cols; ++i) {
                for(int j {0}; j < rows; ++j) {
                    mapTiles[i][j]->setUniforms();
                    mapTiles[i][j]->draw();
                    // Check if a box is on top of a target
                    if(solution[i][j] == 2 && mapState[i][j] == 2) {
                        // Change box color
                        mapTiles[i][j]->setColor(boxOnTarget);
                    }
                }
            }
            // Show pause button hotkey
            fontRenderer->renderText("ESC to pause",
                                     20, (float)height - 30,
                                     0.5, vec3{1, 1, 1});

            // Render moves counter
            fontRenderer->renderText("Moves: " + std::to_string(moves),
                                     (float)width + 60, (float)height - 30,
                                     0.5, vec3{1, 1, 1});
            break;
        }
        case pause: {
            // shadows
            resumeButtonShadow->setUniforms();
            resumeButtonShadow->draw();
            menuButtonShadow->setUniforms();
            menuButtonShadow->draw();
            restartButtonShadow->setUniforms();
            restartButtonShadow->draw();
            // buttons
            resumeButton->setUniforms();
            resumeButton->draw();
            menuButton->setUniforms();
            menuButton->draw();
            restartButton->setUniforms();
            restartButton->draw();

            fontRenderer->renderText("Resume",
                                     resumeButton->getPosX() + (float)(12 * 5.2),
                                     resumeButton->getPosY() - 4,
                                     0.5,
                                     {1, 1, 1});
            fontRenderer->renderText("Menu",
                                     menuButton->getPosX() + (float)(12 * 6.2),
                                     menuButton->getPosY() - 4,
                                     0.5,
                                     {1, 1, 1});
            fontRenderer->renderText("Restart",
                                     quitButton->getPosX() + (float)(12 * 4.7),
                                     quitButton->getPosY() - 4,
                                     0.5,
                                     {1, 1, 1});
            break;
        }
        case levelComplete: {
            // shadows
            nextLevelButtonShadow->setUniforms();
            nextLevelButtonShadow->draw();
            completeMenuButtonShadow->setUniforms();
            completeMenuButtonShadow->draw();
            completeLevelButtonShadow->setUniforms();
            completeLevelButtonShadow->draw();
            // buttons
            nextLevelButton->setUniforms();
            nextLevelButton->draw();
            completeMenuButton->setUniforms();
            completeMenuButton->draw();
            completeLevelButton->setUniforms();
            completeLevelButton->draw();

            // Render button text
            fontRenderer->renderText("Next Level",
                                     nextLevelButton->getPosX() - 1,
                                     nextLevelButton->getPosY() - 4,
                                     0.5,
                                     {1, 1, 1});
            fontRenderer->renderText("Menu",
                                     completeMenuButton->getPosX() + (float)(12 * 5.95),
                                     completeMenuButton->getPosY() - 4,
                                     0.5,
                                     {1, 1, 1});
            fontRenderer->renderText("Levels",
                                     completeLevelButton->getPosX() + (float)(12 * 8.7),
                                     completeLevelButton->getPosY() - 4,
                                     0.5,
                                     {1, 1, 1});

            // Render Level Completed! text
            fontRenderer->renderText("Level Completed!",
                                     ((float)width / 2) - (12 * 6),
                                     ((float)height / 2) + (12 * 8),
                                     1,
                                     vec3{1, 1, 1});
            // Render moves counter
            fontRenderer->renderText("Moves: " + std::to_string(moves),
                                     ((float)width / 2) + (12 * 2),
                                     ((float)height / 2) + (float)(12 * 3.5),
                                     0.75,
                                     vec3{1, 1, 1});
            // Render time elapsed counter (a secret stat! the player doesn't know they are being timed until
            // they complete their first level)
            fontRenderer->renderText("Seconds Elapsed: " + std::to_string(deltaTime),
                                     ((float)width / 2) - (12 * 9),
                                     ((float)height / 2) - (12),
                                     0.75,
                                     vec3{1, 1, 1});

            break;
        }
    }
    glfwSwapBuffers(window);
}

bool Engine::shouldClose() {
    return glfwWindowShouldClose(window);
}

// Helper function to set up a new level
// Reads from ../res/maps.txt
void Engine::initLevel(int level) {
    // Reset the map
    mapTiles.clear();
    mapState.clear();
    mapInit.clear();
    solution.clear();

    ifstream mapFile;
    mapFile.open("../res/maps.txt");

    string mapRow;
    char firstChar;
    // levels are prefixed with a digit
    while(!isdigit(firstChar)) {
        getline(mapFile, mapRow);
        firstChar = mapRow.at(0);
        // make sure we are at the desired level in the file
        if(isdigit(firstChar)) {
            // subtract 48 to adjust for ascii values not matching integers
            if(firstChar - 48 != level) {
                firstChar = '_';
            }
        }
    }

    firstChar = '_';
    // get the first line of the map
    getline(mapFile, mapRow);
    // initialize depth to be the length of the string
    int size = mapRow.length();
    mapTiles.resize(size);
    solution.resize(size);

    mapState.resize(size);
    mapInit.resize(size);

    int col {0}; // used for coordinates
    int row {0}; // used for index and coordinates

    while(!isdigit(firstChar) && !mapFile.eof()) {
        for(char tile : mapRow) {
            // if there are more rows than columns resize accordingly
            // add 1 to adjust for 0 index
            if (size > row) {
                mapTiles.resize(row + 1);
                solution.resize(row + 1);
                mapInit.resize(row + 1);
                mapState.resize(row + 1);
            }
            switch(tile) {
                // floor
                // add walkable to level and add 0 to solution, mapInit, mapState
                // 0 - walkable tile
                case '_': {
                    mapTiles[row].push_back(make_unique<Rect>(shapeShader,
                                                              vec2{(col * 50) + 25, (row * 50) + 25}, // grid of 50x50 tiles
                                                              vec2{50, 50},
                                                              color{1, 1, 1, 1}));
                    solution[row].push_back(0);
                    mapInit[row].push_back(0);
                    mapState[row].push_back(0);
                    ++col;
                    break;
                }
                // wall
                // add wall to level and add 1 to solution, mapInit, mapState
                // 1 - wall tile
                case 'X': {
                    mapTiles[row].push_back(make_unique<Rect>(shapeShader,
                                                              vec2{(col * 50) + 25, (row * 50) + 25}, // grid of 50x50 tiles
                                                              vec2{50, 50},
                                                              color{0.5, 0.5, 0.5, 1}));
                    solution[row].push_back(1);
                    mapInit[row].push_back(1);
                    mapState[row].push_back(1);
                    ++col;
                    break;
                }
                // box
                // add box to level, add 0 to solution and mapInit, add 2 to mapState
                // 0 - walkable tile
                // 2 - box
                case '*': {
                    mapTiles[row].push_back(make_unique<Rect>(shapeShader,
                                                              vec2{(col * 50) + 25, (row * 50) + 25}, // grid of 50x50 tiles
                                                              vec2{50, 50},
                                                              color{0.5, 0.25, 0, 1}));
                    solution[row].push_back(0);
                    mapInit[row].push_back(0);
                    mapState[row].push_back(2);
                    ++col;
                    break;
                }
                // target
                // add target to level and add 2 to solution, 3 to mapInit
                // 2 - box should be here
                // 3 - target
                case '!': {
                    mapTiles[row].push_back(make_unique<Rect>(shapeShader,
                                                              vec2{(col * 50) + 25, (row * 50) + 25}, // grid of 50x50 tiles
                                                              vec2{50, 50},
                                                              color{1, 0.5, 0, 1}));
                    solution[row].push_back(2);
                    mapInit[row].push_back(3);
                    mapState[row].push_back(3);
                    ++col;
                    break;
                }
                // player
                // add player to level and add 0 to solution, mapInit, mapState
                // 0 - walkable tile
                case '@': {
                    mapTiles[row].push_back(make_unique<Rect>(shapeShader,
                                                              vec2{(col * 50) + 25, (row * 50) + 25}, // grid of 50x50 tiles
                                                              vec2{50, 50},
                                                              color{1, 0, 1, 1}));
                    solution[row].push_back(0);
                    mapInit[row].push_back(0);
                    mapState[row].push_back(0);
                    playerPos.col = col;
                    playerPos.row = row;
                    ++col;
                    break;
                }
                // box and target
                // add box to level, 2 to solution and mapState, and 3 to mapInit
                // 2 - box should be here
                // 3 - target
                case '$': {
                    mapTiles[row].push_back(make_unique<Rect>(shapeShader,
                                                              vec2{(col * 50) + 25, (row * 50) + 25}, // grid of 50x50 tiles
                                                              vec2{50, 50},
                                                              color{0.5, 0.25, 0, 1}));
                    solution[row].push_back(2);
                    mapInit[row].push_back(3);
                    mapState[row].push_back(2);
                    ++col;
                    break;
                }
                default: {
                    cout << "invalid character in level " << level << " map: " << mapRow << endl;
                }
            }
        }
        // get the next line
        getline(mapFile, mapRow);
        ++row;
        // reset x
        col = 0;

        // get first char to check if we've reached the next levels data
        firstChar = mapRow.at(0);
    }
    // shrink the map to prevent indexing errors elsewhere
    mapTiles.resize(row);
    mapFile.close();
}

void Engine::tryMovePlayer(const Engine::moveDir &dir) {
    int row = playerPos.row;
    int col = playerPos.col;
    // Navigating the matrices:
    // ex. mapState[row_idx][col_idx]
    // Rows are in ascending order, bottom row is 0
    //
    // 0 - passable
    // 1 - wall
    // 2 - box
    // 3 - target
    // Is the above tile a wall? Do nothing if so
    switch(dir) {
        case moveDir::Up: {
            if(mapState[row + 1][col] != 1) {
                // There is a box, additional logic
                if(mapState[row + 1][col] == 2) {
                    // if there are at least 2 rows above the current row
                    if(row + 2 < mapTiles.size()) {
                        // if the 2nd tile is not a wall or box
                        if(mapState[row + 2][col] != 1 && mapState[row + 2][col] != 2) {
                            // the tile 2 above is now a box
                            mapState[row + 2][col] = 2;
                            mapTiles[row + 2][col]->setColor(boxColor);
                            // update the mapState of the tile the box was in
                            mapState[row + 1][col] = mapInit[row + 1][col];
                            // set upper tile to be the player color
                            mapTiles[row + 1][col]->setColor(playerColor);
                            // set old player tile to be correct color
                            setDefaultTileColor(row, col);
                            // update player position
                            ++playerPos.row;
                            // increment moves counter
                            ++moves;
                            // check solution
                            finishedLevel = checkSolution();
                        }
                    }
                }
                // No box, easy to move
                else {
                    // set upper tile to be the player color
                    mapTiles[row + 1][col]->setColor(playerColor);
                    // set old player tile to be correct color
                    setDefaultTileColor(row, col);
                    // update player position
                    ++playerPos.row;
                    // increment moves counter
                    ++moves;
                }
            }
            break;
        }
        case moveDir::Down: {
            if(mapState[row - 1][col] != 1) {
                // There is a box, additional logic
                if(mapState[row - 1][col] == 2) {
                    // if there are at least 2 rows below the current row
                    if(row - 2 >= 0) {
                        // if the 2nd tile is not a wall or box
                        if(mapState[row - 2][col] != 1 && mapState[row - 2][col] != 2) {
                            // the tile 2 below is now a box
                            mapState[row - 2][col] = 2;
                            mapTiles[row - 2][col]->setColor(boxColor);
                            // update the mapState of the tile the box was in
                            mapState[row - 1][col] = mapInit[row - 1][col];
                            // set lower tile to be the player color
                            mapTiles[row - 1][col]->setColor(playerColor);
                            // set old player tile to be correct color
                            setDefaultTileColor(row, col);
                            // update player position
                            --playerPos.row;
                            // increment moves counter
                            ++moves;
                            // check solution
                            finishedLevel = checkSolution();
                        }
                    }
                }
                // No box, easy to move
                else {
                    // set lower tile to be the player color
                    mapTiles[row - 1][col]->setColor(playerColor);
                    // set old player tile to be correct color
                    setDefaultTileColor(row, col);
                    // update player position
                    --playerPos.row;
                    // increment moves counter
                    ++moves;
                }
            }
            break;
        }
        case moveDir::Right: {
            if(mapState[row][col + 1] != 1) {
                // There is a box, additional logic
                if(mapState[row][col + 1] == 2) {
                    // if there are at least 2 columns right of the current column
                    if(col + 2 < mapState.size()) {
                        // if the 2nd tile is not a wall or box
                        if(mapState[row][col + 2] != 1 && mapState[row][col + 2] != 2) {
                            // the tile 2 right is now a box
                            mapState[row][col + 2] = 2;
                            mapTiles[row][col + 2]->setColor(boxColor);
                            // update the mapState of the tile the box was in
                            mapState[row][col + 1] = mapInit[row][col + 1];
                            // set right tile to be the player color
                            mapTiles[row][col + 1]->setColor(playerColor);
                            // set old player tile to be correct color
                            setDefaultTileColor(row, col);
                            // update player position
                            ++playerPos.col;
                            // increment moves counter
                            ++moves;
                            // check solution
                            finishedLevel = checkSolution();
                        }
                    }
                }
                // No box, easy to move
                else {
                    // set right tile to be the player color
                    mapTiles[row][col + 1]->setColor(playerColor);
                    // set old player tile to be correct color
                    setDefaultTileColor(row, col);
                    // update player position
                    ++playerPos.col;
                    // increment moves counter
                    ++moves;
                }
            }
            break;
        }
        case moveDir::Left: {
            if(mapState[row][col - 1] != 1) {
                // There is a box, additional logic
                if(mapState[row][col - 1] == 2) {
                    // if there are at least 2 columns left of the current column
                    if(col - 2 >= 0) {
                        // if the 2nd tile is not a wall or box
                        if(mapState[row][col - 2] != 1 && mapState[row][col - 2] != 2) {
                            // the tile 2 left is now a box
                            mapState[row][col - 2] = 2;
                            mapTiles[row][col - 2]->setColor(boxColor);
                            // update the mapState of the tile the box was in
                            mapState[row][col - 1] = mapInit[row][col - 1];
                            // set left tile to be the player color
                            mapTiles[row][col - 1]->setColor(playerColor);
                            // set old player tile to be correct color
                            setDefaultTileColor(row, col);
                            // update player position
                            --playerPos.col;
                            // increment moves counter
                            ++moves;
                            // check solution
                            finishedLevel = checkSolution();
                        }
                    }
                }
                // No box, easy to move
                else {
                    // set left tile to be the player color
                    mapTiles[row][col - 1]->setColor(playerColor);
                    // set old player tile to be correct color
                    setDefaultTileColor(row, col);
                    // update player position
                    --playerPos.col;
                    // increment moves counter
                    ++moves;
                }
            }
            break;
        }
    }
}

void Engine::setDefaultTileColor(int row, int col) {
    int tile = mapInit[row][col];
    switch (tile) {
        case 0: {
            mapTiles[row][col]->setColor(walkableColor);
            break;
        }
        case 1: {
            mapTiles[row][col]->setColor(wallColor);
            break;
        }
        case 3: {
            mapTiles[row][col]->setColor(targetColor);
            break;
        }
        default: {
            cout << "invalid character in mapInit" << endl;
            break;
        }
    }
}

bool Engine::checkSolution() {
    for(int i {0}; i < mapState.size(); ++i) {
        for(int j {0}; j < mapState[0].size(); ++j) {
            if(mapState[i][j] != solution[i][j]) {
                return false;
            }
        }
    }
    return true;
}

void Engine::keyCallback(GLFWwindow* m_window, int key, int scancode, int action, int mods) {
    // pause if escape is pressed in play screen
    if (keys[GLFW_KEY_ESCAPE] && screen == play) {
        screen = pause;
    }
    // exit to menu if escape is pressed in levelSelect or levelComplete
    else if (keys[GLFW_KEY_ESCAPE] && (screen == levelSelect || screen == levelComplete)) {
        screen = menu;
    }
    // player movement in the play screen, WASD or arrow keys
    if ((keys[GLFW_KEY_UP] || keys[GLFW_KEY_W]) && screen == play) {
        tryMovePlayer(moveDir::Up);
    }
    else if ((keys[GLFW_KEY_DOWN] || keys[GLFW_KEY_S]) && screen == play) {
        tryMovePlayer(moveDir::Down);
    }
    else if ((keys[GLFW_KEY_RIGHT] || keys[GLFW_KEY_D]) && screen == play) {
        tryMovePlayer(moveDir::Right);
    }
    else if ((keys[GLFW_KEY_LEFT] || keys[GLFW_KEY_A]) && screen == play) {
        tryMovePlayer(moveDir::Left);
    }
}