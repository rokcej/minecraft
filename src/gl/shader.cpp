#include "shader.h"

#include <iostream>
#include <src/utils/file.h>
#include <glm/gtc/type_ptr.hpp>

// TODO: Add geometry shader support
Shader::Shader(const std::string& vertex_path, const std::string& fragment_path) {
	id_ = gl::CreateProgram(vertex_path, fragment_path);
}

Shader::~Shader() {
	glDeleteProgram(id_);
}

void Shader::Use() const {
	glUseProgram(id_);
}

void Shader::SetVector2(const char* name, float x, float y) {
	glUniform2f(glGetUniformLocation(id_, name), x, y);
}

void Shader::SetVector3(const char* name, float x, float y, float z) {
	glUniform3f(glGetUniformLocation(id_, name), x, y, z);
}

void Shader::SetMatrix4(const char* name, const glm::mat4& matrix) {
	glUniformMatrix4fv(glGetUniformLocation(id_, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

namespace gl {

	GLuint CreateShader(const std::string& file_path, GLenum shader_type) {
		std::string shader_source = file::ReadTextFile(file_path);
		const char* shader_source_c = shader_source.c_str();

		GLuint shader = glCreateShader(shader_type);
		glShaderSource(shader, 1, &shader_source_c, NULL);
		glCompileShader(shader);

		GLint is_compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
		if (is_compiled == GL_FALSE) {
			// Compilation error
			GLint log_length;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

			GLchar* log = new GLchar[log_length];
			glGetShaderInfoLog(shader, log_length, &log_length, log);

			std::cerr << "[SHADER COMPILATION ERROR] " << file_path << std::endl;
			std::cerr << log << std::endl << std::endl;
			delete[] log;
		}

		return shader;
	}

	GLuint CreateProgram(const std::string& vertex_shader_path, const std::string& fragment_shader_path) {
		GLuint vertex_shader = CreateShader(vertex_shader_path, GL_VERTEX_SHADER);
		GLuint fragment_shader = CreateShader(fragment_shader_path, GL_FRAGMENT_SHADER);

		GLuint program = glCreateProgram();
		glAttachShader(program, vertex_shader);
		glAttachShader(program, fragment_shader);

		glLinkProgram(program);
		GLint is_linked;
		glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
		if (is_linked == GL_FALSE) {
			// Linking error
			GLint log_length;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

			GLchar* log = new GLchar[log_length];
			glGetProgramInfoLog(program, log_length, &log_length, log);

			std::cerr << "[PROGRAM LINKING ERROR] " << vertex_shader_path << ", " << fragment_shader_path << std::endl;
			std::cerr << log << std::endl << std::endl;
			delete[] log;
		}
		else {
			// Linking success
#ifdef DEBUG
			glValidateProgram(program);
			GLint is_valid;
			glGetProgramiv(program, GL_VALIDATE_STATUS, &is_valid);
			if (is_valid == GL_FALSE) {
				// Validation error
				GLint log_length;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

				GLchar* log = new GLchar[log_length];
				glGetProgramInfoLog(program, log_length, &log_length, log);

				std::cerr << "[PROGRAM VALIDATION ERROR] " << vertex_shader_path << ", " << fragment_shader_path << std::endl;
				std::cerr << log << std::endl << std::endl;
				delete[] log;
			}
#endif
		}

		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		return program;
	}

}
