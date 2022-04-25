#include "ShaderProgram.h"
#include <fstream>
#include <iostream>

using namespace GL;

ShaderProgram::ShaderProgram()
{
}

void ShaderProgram::init(const char* vertexFile, const char* fragmentFile)
{
	vertexShaderID = loadShader(vertexFile, GL_VERTEX_SHADER);
	fragmentShaderID = loadShader(fragmentFile, GL_FRAGMENT_SHADER);
	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	bindAttributes();
	glLinkProgram(programID);
	glValidateProgram(programID);
	getAllUniformLocations();
}

void ShaderProgram::start()
{
	glUseProgram(programID);
}

void ShaderProgram::stop()
{
	glUseProgram(0);
}

void ShaderProgram::cleanUp()
{
	stop();
	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	glDeleteProgram(programID);
}

void ShaderProgram::bindAttribute(int attribute, const char* variableName)
{
	glBindAttribLocation(programID, attribute, variableName);
}

int ShaderProgram::getUniformLocation(const char* uniformName)
{
	GLint location = glGetUniformLocation(programID, uniformName);
	if (location == -1)
	{
		printf("Error finding %s in program %d\n", uniformName, programID);
	}
	return location;
}

void ShaderProgram::loadMatrix(int location, glm::mat4 matrix)
{
	glUniformMatrix4fv(location, 1, GL_TRUE, &matrix[0][0]);
}

void ShaderProgram::loadInt(int location, int value)
{
	glUniform1i(location, value);
}

void ShaderProgram::loadBoolean(int location, bool value)
{
	float toLoad = value ? 1.f : 0.f;
	glUniform1f(location, toLoad);
}

void ShaderProgram::loadFloat(int location, float value)
{
	glUniform1f(location, value);
}

void ShaderProgram::loadVector(int location, glm::vec2 vector)
{
	glUniform2f(location, vector.x, vector.y);
}

void ShaderProgram::loadVector(int location, glm::vec3 vector)
{
	glUniform3f(location, vector.x, vector.y, vector.z);
}

void ShaderProgram::loadVector(int location, glm::vec4 vector)
{
	glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}

GLuint ShaderProgram::loadShader(const char* file, GLenum type)
{
	GLuint shaderID = glCreateShader(type);
	std::string shaderBasePath = "./shaders/";
	std::string fileName = shaderBasePath.append(file);
	std::ifstream fileStream(fileName, std::ios::in);
	std::string content((std::istreambuf_iterator<char>(fileStream)), (std::istreambuf_iterator<char>()));
	const GLchar* buffer = content.c_str();


	glShaderSource(shaderID, 1, &buffer, nullptr);
	glCompileShader(shaderID);
	GLint compileResult;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE)
	{
		printf("Error compiling:\n");
	}
#ifndef NDEBUG
	char log[1000];
	GLint length;
	glGetShaderInfoLog(shaderID, 1000, &length, log);
	printf("%s", log);
#endif //!NDEBUG
	return shaderID;
}
