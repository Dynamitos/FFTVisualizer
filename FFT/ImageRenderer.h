#pragma once
#include "Resources.h"
#include "ImageShader.h"

class Camera;
class RawModel;
class OpenGLLoader;

class ImageRenderer
{
public:
	ImageRenderer();
	void init(OpenGLLoader* loader, AudioVisualizerInfo visualizerInfo);
	void render(float bass);
	
private:
	glm::mat4 transformationMatrix;
	glm::mat4 projectionMatrix;
	ImageShader* shader;
	bool isScaling;
	GLint textureID;
	Camera* camera;

	const float FOV = 70.f;
	const float NEAR_PLANE = 1.f;
	const float FAR_PLANE = 1000.f;

	void createProjectionMatrix();
};