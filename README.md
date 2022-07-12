# Minecraft

My attempt at recreating Minecraft using C++ and OpenGL.


## Building

```bash
mkdir build
cmake -S . -B build
cmake --build build
```

## Dependencies

| Dependency | Version |
| ---------- | ------: |
| GLFW       | 3.3.7   |
| GLAD       | 0.1.35  |
| glm        | 0.9.9.8 |
| stb_image  | 2.27    |
| FreeType   | 2.12.1  |


## Future improvements

* Improved game loop:
	* https://dewitters.com/dewitters-gameloop/
	* https://gafferongames.com/post/fix_your_timestep/


## References

* LearnOpenGL - https://learnopengl.com
* How to read in a file in C++ - https://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
* Optimized Spatial Hashing for Collision Detection of Deformable Objects - http://www.beosil.com/download/CollisionDetectionHashing_VMV03.pdf
* Improved Alpha-Tested Magnification for Vector Textures and Special Effects - https://steamcdn-a.akamaihd.net/apps/valve/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf


## Similar projects

* Minecraft in C++ & OpenGL - https://github.com/Hopson97/MineCraft-One-Week-Challenge
* Minecraft in C & OpenGL - https://github.com/jdah/minecraft-weekend


## Notes

* Setting working directory for Visual Studio debugging:
	1. Right click on target executable, select "Add Debug Configuration"
	2. Add field `"currentDir": "${workspaceRoot}"`
* FreeType tips:
	* `FT_Load_Char(FT_LOAD_RENDER) --> FT_Get_Glyph()` is faster than `FT_Load_Char(FT_LOAD_DEFAULT) --> FT_Load_Char(FT_LOAD_RENDER)` (~24% performance increase over 10000 iterations)
	* `glTexImage2D() --> glTexSubImage2D()` is faster than `std::copy() --> glTexImage2D()` (~2.1x faster over a single iteration)

## Benchmarks

* Font rendering (201 texts)
	1. Rendering each character using separate texture: 106 FPS
	1. Rendering each character using font atlas: 117 FPS
	1. Rendering whole text using font atlas: ~3800 FPS
