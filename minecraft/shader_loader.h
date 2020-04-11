#pragma once

#include <string>
#include <GL/glew.h>

namespace ShaderLoader {
	GLuint compileProgram(std::string vertexShaderPath, std::string fragmentShaderPath);
}
