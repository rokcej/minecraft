#pragma once

#include <string>
#include <glad/glad.h>

namespace gl {

	GLuint CreateShader(const std::string& shader_path, GLenum shader_type);
	GLuint CreateProgram(const std::string& vertex_shader_path, const std::string& fragment_shader_path);

}
