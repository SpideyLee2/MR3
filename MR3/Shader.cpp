#include "Shader.h"

Shader::Shader(const char* vertexShaderDir, const char* fragmentShaderDir) {
	std::stringstream vertexSource, fragmentSource;
	try {
		std::ifstream vertFile, fragFile;
		vertFile.open(vertexShaderDir);
		fragFile.open(fragmentShaderDir);

		vertexSource << vertFile.rdbuf();
		fragmentSource << fragFile.rdbuf();

		vertFile.close();
		fragFile.close();

	} catch (std::ifstream::failure e) {
		std::cout << "Failed to read shader source files at either: " << vertexShaderDir << " or " << fragmentShaderDir
			<< std::endl;
	}

	std::string vCode = vertexSource.str();
	std::string fCode = fragmentSource.str();

	const char* vertexCode = vCode.c_str();
	const char* fragmentCode = fCode.c_str();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexCode, NULL);
	glCompileShader(vertexShader);
	checkShaderComp(vertexShader, "VERTEX");

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentCode, NULL);
	glCompileShader(fragmentShader);
	checkShaderComp(fragmentShader, "FRAGMENT");

	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	glLinkProgram(id);
	checkProgramLink(id, "SHADER");

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::use() const {
	glUseProgram(id);
}

void Shader::checkShaderComp(GLuint shader, const char* type) {
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, 0, infoLog);
		std::cout << "ERROR-SHADER-" << type << "-COMPILATION_FAILED" << std::endl << infoLog << std::endl;
	}
}

void Shader::checkProgramLink(GLuint program, const char* type) {
	int success;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, 0, infoLog);
		std::cout << "ERROR-PROGRAM-" << type << "-LINKING_FAILED" << std::endl << infoLog << std::endl;
	}
}