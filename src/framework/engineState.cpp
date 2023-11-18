#include "engineState.h"

// without this line there is a compiler error: undefined reference to 'EngineState::eventHandlingInstance'
EngineState * EngineState::eventHandlingInstance; // initializes the abstract event listener

void EngineState::setEventHandling() { eventHandlingInstance = this; }

void EngineState::keyCallbackDispatch(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if(eventHandlingInstance)
        eventHandlingInstance->keyCallback(window, key, scancode, action, mods);
}