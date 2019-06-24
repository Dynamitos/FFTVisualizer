#include "ImageShader.h"

using namespace GL;

ImageShader::ImageShader()
{
	init(VERT_FILE, FRAG_FILE);
}

void ImageShader::loadIntensityMultipliers(float intensityScale, float intensityOffset)
{
	ShaderProgram::loadFloat(location_intensityScale, intensityScale);
	ShaderProgram::loadFloat(location_intensityOffset, intensityOffset);
}

void ImageShader::loadBass(float bass)
{
	ShaderProgram::loadFloat(location_bass, bass);
}

void ImageShader::loadTransformationMatrix(glm::mat4 transformationMatrix)
{
	ShaderProgram::loadMatrix(location_transformationMatrix, transformationMatrix);
}

void ImageShader::loadViewMatrix(glm::mat4 viewMatrix)
{
	ShaderProgram::loadMatrix(location_view, viewMatrix);
}

void ImageShader::loadTextures()
{
	ShaderProgram::loadInt(location_texture, 0);
}

void ImageShader::loadLight(glm::vec3 lightPosition, glm::vec4 lightColor)
{
	ShaderProgram::loadVector(location_lightPosition, lightPosition);
	ShaderProgram::loadVector(location_lightColor, lightColor);
}

void ImageShader::loadProjection(glm::mat4 projection)
{
	ShaderProgram::loadMatrix(location_projection, projection);
}

void ImageShader::getAllUniformLocations()
{
	location_texture = ShaderProgram::getUniformLocation("textureSampler");
	location_transformationMatrix = ShaderProgram::getUniformLocation("transformationMatrix");
	location_lightPosition = ShaderProgram::getUniformLocation("lightPosition");
	location_lightColor = ShaderProgram::getUniformLocation("lightColor");
	location_lineColor = ShaderProgram::getUniformLocation("lineColor");
	location_intensityScale = ShaderProgram::getUniformLocation("intensityScale");
	location_intensityOffset = ShaderProgram::getUniformLocation("intensityOffset");
	location_bass = ShaderProgram::getUniformLocation("bass");
	location_lineWidth = ShaderProgram::getUniformLocation("lineWidth");
	location_array = ShaderProgram::getUniformLocation("music");
	location_projection = ShaderProgram::getUniformLocation("projection");
	location_view = ShaderProgram::getUniformLocation("viewMatrix");
}

void ImageShader::bindAttributes()
{
	//use vertexless render
//	ShaderProgram::bindAttribute(0, "position");
//	ShaderProgram::bindAttribute(1, "texCoords");
}
