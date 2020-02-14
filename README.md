# Minecraft

My attempt at recreating Minecraft.

## Configuration

Make sure the project configuration is set to `x64 Release`.

## Dependencies

All dependencies should be 64bit.

| Dependency | Version | Description             |
| ---------- | ------: | ----------------------- |
| GLFW       | 3.3.2   | OpenGL context creation |
| GLEW       | 2.1.0   | OpenGL extention loader |
| GLM        | 0.9.9.7 | OpenGL math library     |
| libpng     | 1.6.37  | PNG reference library   |
| zlib       | 1.2.11  | libpng dependency       |

Visual Studio will look for dependencies in the following directories relative to the project:
* `dependencies/bin/x64/` - Compiled binaries (`.dll`)
* `dependencies/lib/x64/` - Library files (`.lib`)
* `dependencies/include/` - Include files (`.h`/`.hpp`)
