#pragma once

#include <glad/glad.h>

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

class Shader {
public:
	GLuint id;

	Shader(const char* vertexShaderDir, const char* fragmentShaderDir);
	void use() const;

private:
	void checkShaderComp(GLuint shader, const char* type);
	void checkProgramLink(GLuint program, const char* type);
};