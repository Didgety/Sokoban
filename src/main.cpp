#include "framework/engine.h"
#include "framework/engineState.h"

int main(int argc, char *argv[]) {
    Engine engine;
    // activate event listener for keyboard input
    engine.setEventHandling();

    while (!engine.shouldClose()) {
        engine.processInput();
        engine.update();
        engine.render();
    }

    glfwTerminate();
    return 0;
}