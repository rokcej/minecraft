#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace gl {

	GLuint CreateShader(const std::string& file_path, GLenum shader_type);
	GLuint CreateProgram(const std::string& vertex_shader_path, const std::string& fragment_shader_path);

}

class Shader {
public:
	Shader(const std::string& vertex_path, const std::string& fragment_path);
	~Shader();

	void Use() const;

	// Using `const char*` is much faster than `const string&`
	void SetVector2(const char* name, float x, float y);
	void SetVector3(const char* name, float x, float y, float z);
	void SetMatrix4(const char* name, const glm::mat4& matrix);

private:
	GLuint id_;

};
