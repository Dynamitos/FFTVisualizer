#pragma once
#include "Resources.h"
namespace GL
{
	class ShaderProgram
	{
	public:
		ShaderProgram();
		void init(const char* vertexFile, const char* fragmentFile);
		void start();
		void stop();
		void cleanUp();
	protected:
		void bindAttribute(int attribute, const char* variableName);
		virtual void getAllUniformLocations() = 0;
		GLint getUniformLocation(const char* uniformName);

		virtual void bindAttributes() = 0;

		void loadMatrix(int location, glm::mat4 matrix);
		void loadInt(int location, int value);
		void loadBoolean(int location, bool value);
		void loadFloat(int location, float value);
		void loadVector(int location, glm::vec2 vector);
		void loadVector(int location, glm::vec3 vector);
		void loadVector(int location, glm::vec4 vector);
	private:
		GLuint programID;
		GLuint vertexShaderID;
		GLuint fragmentShaderID;

		static GLuint loadShader(const char* file, GLenum type);
	};
}