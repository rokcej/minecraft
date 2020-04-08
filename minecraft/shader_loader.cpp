#include "shader_loader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include "defines.h"

// http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
std::string readFile(std::string filename) {
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (!in) {
		std::cerr << "ERROR: Can't read file " << filename << std::endl;
		return "";
	}
	std::ostringstream oss;
	oss << in.rdbuf();
	return oss.str();
}

GLuint compileProgram(std::string vertexShaderPath, std::string fragmentShaderPath) {
	// Read shaders
	std::string vertexShaderSource = readFile(SHADER_DATA_PATH + vertexShaderPath);
	std::string fragmentShaderSource = readFile(SHADER_DATA_PATH + fragmentShaderPath);
	const char* vsSrc = vertexShaderSource.c_str();
	const char* fsSrc = fragmentShaderSource.c_str();

	// Error log
	GLint status;
	char buf[1024];

	// Vertex shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vsSrc, NULL);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		glGetShaderInfoLog(vs, sizeof(buf), NULL, buf);
		std::cerr << "[Vertex shader compile error log]" << std::endl << buf << std::endl;
	}

	// Fragment shader
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fsSrc, NULL);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		glGetShaderInfoLog(fs, sizeof(buf), NULL, buf);
		std::cerr << "[Fragment shader compile error log]" << std::endl << buf << std::endl;
	}

	// Program
	GLuint prog = glCreateProgram();
	glAttachShader(prog, vs);
	glAttachShader(prog, fs);
	glLinkProgram(prog);
	glGetProgramiv(prog, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		glGetProgramInfoLog(prog, sizeof(buf), NULL, buf);
		std::cerr << "[Program link error log]" << std::endl << buf << std::endl;
	}
	glValidateProgram(prog);
	glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
	if (status == GL_FALSE) {
		glGetProgramInfoLog(prog, sizeof(buf), NULL, buf);
		std::cerr << "[Program validate error log]" << std::endl << buf << std::endl;
	}

	// Cleanup
	glDeleteShader(vs);
	glDeleteShader(fs);

	return prog;
}
