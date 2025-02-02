<h1 align="center">Ruya</h1>
A general purpose graphics engine built using OpenGL. Created for learning purposes.

**Definition attempt (may change)**

The goal of the Ruya project is to provide an intuitive and simple rendering engine, which is easy to use and get started with.

The main components are:
* `Ruya::Scene`: logic responsible for defining the scene to render.
* `Ruya::Render`: logic responsible for rendering the scene.
* `Ruya::UI`: logic responsible for creating and interacting with the userinterface.

## Quickstart

## Building
TODO
- dependencies
- how is the project structured

### Building on Linux
TODO

### Building on Windows
In progress
Take inspiration from: https://github.com/Eagle-E/imguitest
- (cmake project with glad, glfw and imgui as dependencies)


Execute in the project root, in the folder where this `readme.md` file is located: 
```
cmake -S . -B build -G "Ninja"
```