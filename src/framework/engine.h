#ifndef GRAPHICS_ENGINE_H
#define GRAPHICS_ENGINE_H

#include <vector>
#include <memory>
#include <iostream>
#include <tuple>
#include <GLFW/glfw3.h>

#include "engineState.h"
#include "shaderManager.h"
#include "fontRenderer.h"
#include "../shapes/rect.h"
#include "../shapes/shape.h"
#include "debug.h"

using std::tuple, std::get, std::unique_ptr, std::make_unique, glm::ortho, glm::mat4, glm::vec3, glm::vec4;
/**
 * @brief The Engine class.
 * @details The Engine class is responsible for initializing the GLFW window, loading shaders, and rendering the game state.
 *          it inherits from EngineState which is responsible for providing the necessary backend to implement the
 *          GLFW event listener.
 * @see EngineState
 */
class Engine : public EngineState {
    private:
        /// @brief The actual GLFW window.
        GLFWwindow* window{};

        /// @brief The width and height of the window.
        const unsigned int width = 600, height = 600; // Window dimensions (12x12 max level size)

        /// @brief Keyboard state (True if pressed, false if not pressed).
        /// @details Index this array with GLFW_KEY_{key} to get the state of a key.
        bool keys[1024];

        /// @brief Responsible for loading and storing all the shaders used in the project.
        /// @details Initialized in initShaders()
        unique_ptr<ShaderManager> shaderManager;

        /// @brief Responsible for rendering text on the screen.
        /// @details Initialized in initShaders()
        unique_ptr<FontRenderer> fontRenderer;

        // Instead of creating a bunch of buttons using unique_ptrs that persist until
        // the program ends, I could theoretically just reuse a few buttons. Having
        // their labels and functionality change depending on the context.
        //
        // The downside would be that the buttons names would be generic and not descriptive. Functionality
        // would have to be ascertained from documentation and the surrounding code.
        //
        // For a small program like this, I'm ok making that tradeoff.

        // Shapes
        unique_ptr<Shape> startButton;                          // main menu
        unique_ptr<Shape> startButtonShadow;                    // main menu
        unique_ptr<Shape> levelSelectButton;                    // main menu
        unique_ptr<Shape> levelSelectButtonShadow;              // main menu
        unique_ptr<Shape> quitButton;                           // main menu
        unique_ptr<Shape> quitButtonShadow;                     // main menu
        unique_ptr<Shape> continueButton;                       // instructions
        unique_ptr<Shape> continueButtonShadow;                 // instructions
        vector<unique_ptr<Shape>> levelSelectButtons;           // level select
        vector<unique_ptr<Shape>> levelSelectButtonsShadows;    // level select
        unique_ptr<Shape> levelMenuButton;                      // level select
        unique_ptr<Shape> levelMenuButtonShadow;                // level select
        unique_ptr<Shape> nextLevelButton;                      // level complete
        unique_ptr<Shape> nextLevelButtonShadow;                // level complete
        unique_ptr<Shape> completeMenuButton;                   // level complete
        unique_ptr<Shape> completeMenuButtonShadow;             // level complete
        unique_ptr<Shape> completeLevelButton;                  // level complete
        unique_ptr<Shape> completeLevelButtonShadow;            // level complete

        unique_ptr<Shape> menuButton;                           // pause
        unique_ptr<Shape> menuButtonShadow;                     // pause
        unique_ptr<Shape> resumeButton;                         // pause
        unique_ptr<Shape> resumeButtonShadow;                   // pause
        unique_ptr<Shape> restartButton;                        // pause
        unique_ptr<Shape> restartButtonShadow;                  // pause
        unique_ptr<Rect> player;                                // instructions
        unique_ptr<Rect> wall;                                  // instructions
        unique_ptr<Rect> walkable;                              // instructions
        unique_ptr<Rect> box;                                   // instructions
        unique_ptr<Rect> target;                                // instructions

        vector<vector<unique_ptr<Shape>>> mapTiles; // graphical representation of mapState
        // 0 - passable
        // 1 - wall
        // 2 - box
        // 3 - target
        vector<vector<int>> mapInit;  // initial map state (used to change tiles back to their original color)
        /* Maps are displayed in the horizontal inverse
        *  i.e. the bottom line is actually the top when rendered
        *
        *  Example: Level 1 (playable area)
        *
        *  1 1 1 1 1 1 1 1
        *  1 0 0 0 3 0 0 1
        *  1 0 0 3 0 0 3 1
        *  1 0 1 0 3 0 1 1
        *  1 3 1 1 0 0 1 1
        *  1 1 1 0 0 3 1 1
        *  1 3 0 0 0 0 1 1
        *  1 1 1 0 0 0 1 1
        *  1 1 1 1 1 1 1 1
        */
        vector<vector<int>> mapState; // current map state
        /* Maps are displayed in the horizontal inverse
         *  i.e. the bottom line is actually the top when rendered
         *
         *  Example: Level 1 (playable area)
         *
         *  1 1 1 1 1 1 1 1
         *  1 0 0 0 3 0 0 1
         *  1 2 0 2 2 2 3 1
         *  1 0 1 0 3 0 1 1
         *  1 3 1 1 2 0 1 1
         *  1 1 1 0 2 3 1 1
         *  1 3 0 2 0 0 1 1
         *  1 1 1 0 0 0 1 1
         *  1 1 1 1 1 1 1 1
         */
        vector<vector<int>> solution; // map state should equal this
        /* Maps are displayed in the horizontal inverse
        *  i.e. the bottom line is actually the top when rendered
        *
        *  Example: Level 1 (playable area)
        *
        *  1 1 1 1 1 1 1 1
        *  1 0 0 0 2 0 0 1
        *  1 0 0 2 0 0 2 1
        *  1 0 1 0 2 0 1 1
        *  1 2 1 1 0 0 1 1
        *  1 1 1 0 0 2 1 1
        *  1 2 0 0 0 0 1 1
        *  1 1 1 0 0 0 1 1
        *  1 1 1 1 1 1 1 1
        */

        // Shaders
        Shader shapeShader;
        Shader textShader;

        // Mouse information
        bool mousePressedLastFrame;
        double MouseX, MouseY;

        // Game information
        const int MAX_LEVEL = 5; // keeping this manually updated is fine (also update completedLevels[])

        /* deltaTime variables */
        float startTime {0.0f}; // when play screen is entered
        float endTime {0.0f}; // when levelComplete screen is entered
        float deltaTime {0.0f}; // actual time elapsed playing level

        // Player stats
        int moves {0};
        bool finishedLevel {false}; // is the current level won?
        bool completedLevels [5] {false}; // levels beaten this session (for graphics)

        // players current level, increments on levelComplete. Changed when choosing a level via levelSelect.
        int currLevel{1};

        // Where in the world is Carmen Sandiego? (player coordinates in the matrix)
        struct position {
            int col;
            int row;
        };
        // actual position of the player in the game matrix
        position playerPos;

        // makes it easier to know which direction the player wants to move
        // use enum class to avoid implicit type conversion
        enum class moveDir {
            Up,
            Down,
            Left,
            Right
        };

        /// @brief Helper function to set up a level
        /// @inputs int level - the level number to set up
        /// @details Levels start at 1 and go up to MAX_LEVEL. This function takes an input level number
        ///          and sets up the graphics, mapInit, mapState, and solution matrices.
        ///          Reads from a file ../res/maps.txt
        void initLevel(int level);

        /// @brief Attempts to move the player in a given direction
        /// @inputs enum moveDir - desired direction
        /// @details This function takes an input direction and tries to move the player. If there is a box,
        ///          additional logic checks the next tile to see if the box can be pushed. If a box is
        ///          successfully pushed, checkSolution() is invoked to see if the level is completed
        /// @see Engine::checkSolution()
        void tryMovePlayer(const moveDir &dir);

        /// @brief Helper function to change tile color when moving
        /// @inputs int row, int col - the index of the tile in the matrix whose color needs to be restored
        /// @details This function takes an index and restores the tiles color to its original state of
        ///          walkable or target when the player moves.
        void setDefaultTileColor(int row, int col);

        /// @brief Checks if the current map state is the same as the solution
        /// @details This function should be triggered whenever the player pushes a box. It compares the mapState
        ///          matrix to the solution matrix. If they are equal, proceed to levelComplete screen.
        /// @return true if mapState = solution, false otherwise
        bool checkSolution();

        /// @brief Implements the functionality for glfw keyboard listener
        /// @details Registers keyboard inputs for player movement and tries to
        ///          move the player when in the play screen
        /// @see EngineState::keyCallback()
        void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;

    public:
        /// @brief Constructor for the Engine class.
        /// @details Initializes window and shaders.
        Engine();

        /// @brief Destructor for the Engine class.
        ~Engine();

        /// @brief Initializes the GLFW window.
        /// @return 0 if successful, -1 otherwise.
        unsigned int initWindow(bool debug = false);

        /// @brief Loads shaders from files and stores them in the shaderManager.
        /// @details Renderers are initialized here.
        void initShaders();

        /// @brief Initializes the shapes to be rendered.
        void initShapes();

        /// @brief Processes input from the user.
        /// @details (e.g. keyboard input, mouse input, etc.)
        void processInput();

        /// @brief Updates the game state.
        /// @details (e.g. collision detection, delta time, etc.)
        void update();

        /// @brief Renders the game state.
        /// @details Displays/renders objects on the screen.
        void render();

        // -----------------------------------
        // Getters
        // -----------------------------------

        /// @brief Returns true if the window should close.
        /// @details (Wrapper for glfwWindowShouldClose()).
        /// @return true if the window should close
        /// @return false if the window should not close
        bool shouldClose();

        /// @brief Projection matrix used for 2D rendering (orthographic projection).
        /// @details OpenGL uses the projection matrix to map the 3D scene to a 2D viewport.
        /// @details The projection matrix transforms coordinates in the camera space into
        /// normalized device coordinates. (view space to clip space).
        /// @note The projection matrix is used in the vertex shader.
        /// @note We don't have to change this matrix since the screen size never changes.
        mat4 PROJECTION = ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), -1.0f, 1.0f);
};

#endif //GRAPHICS_ENGINE_H