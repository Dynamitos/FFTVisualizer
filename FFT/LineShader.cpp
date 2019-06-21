#include "LineShader.h"

GL::LineShader::LineShader()
{
	init(VERT_FILE, FRAG_FILE);
}

void GL::LineShader::loadColor(glm::vec4 color)
{
	ShaderProgram::loadVector(location_color, color);
}

void GL::LineShader::getAllUniformLocations()
{
	location_color = ShaderProgram::getUniformLocation("color");
}

void GL::LineShader::bindAttributes()
{
	ShaderProgram::bindAttribute(0, "basePosition");
	ShaderProgram::bindAttribute(1, "displacement");
}
