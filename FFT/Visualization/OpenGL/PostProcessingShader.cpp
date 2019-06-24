#include "PostProcessingShader.h"
using namespace GL;

PostProcessingShader::PostProcessingShader()
{
	init(VERT_FILE, FRAG_FILE);
}

PostProcessingShader::~PostProcessingShader()
{
}

void PostProcessingShader::getAllUniformLocations()
{
	location_texture = ShaderProgram::getUniformLocation("textureSampler");
	location_time = ShaderProgram::getUniformLocation("time");
	location_overlay = ShaderProgram::getUniformLocation("overlay");
	location_text = ShaderProgram::getUniformLocation("text");
}

void PostProcessingShader::bindAttributes()
{
	//use vertexless render
//	ShaderProgram::bindAttribute(0, "position");
//	ShaderProgram::bindAttribute(1, "texCoords");
}

void PostProcessingShader::loadTime(float time)
{
	ShaderProgram::loadFloat(location_time, time);
}

void PostProcessingShader::loadTexture()
{
	ShaderProgram::loadInt(location_texture, 0);
	ShaderProgram::loadInt(location_overlay, 1);
	ShaderProgram::loadInt(location_text, 2);
}
