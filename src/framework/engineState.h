#ifndef SOKOBAN_ENGINESTATE_H
#define SOKOBAN_ENGINESTATE_H

#include <GLFW/glfw3.h>

/**
 * @brief The EngineState class.
 * @details The EngineState class is responsible for providing functionality for child classes to implement the
 *          glfw event listener.
 * @see Engine
 */
class EngineState {
public:
    // Initial naive implementation from: https://www.glfw.org/docs/3.3/input_guide.html
    // Amazingly helpful StackOverflow explaining how to make it work inside a class:
    // https://stackoverflow.com/questions/21799746/how-to-glfwsetkeycallback-for-different-classes
    // virtual callback function
    /// @brief virtual callback function for glfw event listener
    /// @details Implement behavior for keyboard events using this function and keyCallbackDispatch to create
    ///          the event listener.
    /// @see EngineState::keyCallbackDispatch()
    virtual void keyCallback(GLFWwindow *window,
                             int key,
                             int scancode,
                             int action,
                             int mods) = 0; /* purely abstract function */

    // pointer to an instance of the class that will handle the events
    static EngineState *eventHandlingInstance;

    /// @brief Sets the event handler to be associated with the instantiated class
    /// @details Activates event handler for a given instance of a child class.
    ///          Technically setEventHandling should be finalized so that it doesn't
    ///          get overwritten by a descendant class.
    virtual void setEventHandling();

    /// @brief The callback function to be used by the child class for glfw event listening
    /// @details Ensure you are using EngineState::keyCallbackDispatch as the callback function
    ///          in glfwSetKeyCallBack() in the child class  or the event listener will not work.
    static void keyCallbackDispatch(GLFWwindow *window,
                                    int key,
                                    int scancode,
                                    int action,
                                    int mods);

};

#endif //SOKOBAN_ENGINESTATE_H