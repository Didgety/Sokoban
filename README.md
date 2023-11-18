# Sokoban!
#### Jai Veilleux

A recreation of the classic computer game [Sokoban](https://en.wikipedia.org/wiki/Sokoban) in C++. It utilizes the GLFW,
GLAD, GLM, STB, and FreeType libraries.

## Sokoban

Sokoban is a puzzle game about pushing boxes onto the appropriate tiles. You can play
it [here](http://borgar.net/programs/sokoban/) or [here](https://www.mathsisfun.com/games/sokoban.html).

[![Sokoban-ani.gif](Sokoban_ani.gif)](https://en.wikipedia.org/wiki/Sokoban)

## Required Software

- CLion
  - After cloning the repository, open the folder as a CLion project
  - Once CLion has finished loading, use `Reload CMake Project` in the upper left menu
  - The run configurations will now populate, you want to use `Sokoban`

This git repository should install and configure the following submodules:
- [GLAD](https://github.com/Dav1dde/glad.git)
- [GLM](https://github.com/g-truc/glm.git)
- [GLFW](https://github.com/glfw/glfw.git)
- [FreeType](https://github.com/freetype/freetype.git)
- [STB](https://github.com/nothings/stb.git)

### Gameplay

The player spawns in a grid-based map system consisting of immovable walls, passable floors,
and pushable boxes. Each box has a designated tile it needs to be pushed to in order to complete
the level. Move using WASD or the arrow keys.

### Features

- Main menu
  - Start game button
  - Level selection button
  - Quit button
- Level selection
  - Buttons for individual levels
    - Buttons and shadows change color if you have beaten the level
- Play screen
  - Gameplay loop
  - Boxes change color when on a target tile
  - Moves counter
  - ESC to access pause menu
- Pause screen
  - Resume button
  - Exit to menu button
  - Restart button
- Level completed
  - Moves to complete
  - Time to complete
  - Next level
  - Main menu
  - Level select

## Events

- Keyboard Input
  - Implemented GLFW event listener for keyboard input
    - Player movement (WASD, arrow keys)
    - Access pause menu (ESC key in play screen)
    - Access main menu (ESC key in levelSelect or levelComplete screen)
- Mouse Input
  - Clicking on buttons (all screens except play)
    - Changes their color and has context-dependent behavior
  - All buttons change color when hovered or clicked
- Non-Input-Based events
  - levelComplete screen is triggered when mapState = solution
  - Boxes change color when on a target
  - Buttons on levelSelect screen change color when you beat a level

## Bugs
- None known at this time

## Future Work
- More levels
  - Procedural level generation
- Calculate minimum number of moves required for each level and add a score system
  - Closer to the minimum = more points, stars, etc.
- Effects on screen transitions
- Animations for movement
- Saving / loading via serialization / a save file

## References
- [Graphics-Starter](https://github.com/uvmcs2300f2023/Graphics-Starter)
- [GLFW Input Guide](https://www.glfw.org/docs/3.2/input_guide.html#input_keyboard)
  - [GLFW keyCallback for classes](https://stackoverflow.com/questions/21799746/how-to-glfwsetkeycallback-for-different-classes)
- [`Sokoban_ani.gif`](https://en.wikipedia.org/wiki/Sokoban) from Wikipedia
- Code provided by Professor Lisa Dion:
  - All files in `../res/fonts`, `../res/shaders`, and `../res/textures`
  - All files in `../src/framework` except for: `engine.h`, `engine.cpp`, `engineState.h` and `engineState.cpp`
  - All files in `../src/framework`
  - `../CMakeLists.txt`
  - A simple version of `engine.h` and `engine.cpp` implementing the public functions