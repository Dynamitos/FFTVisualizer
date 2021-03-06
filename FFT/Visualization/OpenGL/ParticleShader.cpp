#include "ParticleShader.h"

using namespace GL;

ParticleShader::ParticleShader()
{
	init(VERT_FILE, FRAG_FILE);
}

void ParticleShader::loadViewMatrix(glm::mat4 viewMatrix)
{
	ShaderProgram::loadMatrix(location_viewMatrix, viewMatrix);
}

void ParticleShader::loadProjectionMatrix(glm::mat4 projectionMatrix)
{
	ShaderProgram::loadMatrix(location_projectionMatrix, projectionMatrix);
}

void ParticleShader::loadTexture()
{
	ShaderProgram::loadInt(location_texture, 0);
}

void ParticleShader::getAllUniformLocations()
{
	location_texture = ShaderProgram::getUniformLocation("tex");
	location_projectionMatrix = ShaderProgram::getUniformLocation("projectionMatrix");
	location_viewMatrix = ShaderProgram::getUniformLocation("viewMatrix");
}


void ParticleShader::bindAttributes()
{
	ShaderProgram::bindAttribute(0, "vertex_VS_in");
	ShaderProgram::bindAttribute(1, "positionScale_VS_in");
	ShaderProgram::bindAttribute(2, "rotation_VS_in");
}
