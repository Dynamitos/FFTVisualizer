#include "ImageRenderer.h"
#include "OpenGLLoader.h"

ImageRenderer::ImageRenderer()
{
}

void ImageRenderer::init(OpenGLLoader* loader, AudioVisualizerInfo visualizerInfo)
{
	isScaling = visualizerInfo.scaling;
	textureID = loader->loadTexture(visualizerInfo.imageURL);
	createProjectionMatrix();
	shader = new ImageShader();
	shader->start();
	shader->loadProjection(projectionMatrix);
	shader->loadIntensity(visualizerInfo.intensityScale, visualizerInfo.intensityOffset);
	shader->stop();
	transformationMatrix = glm::mat4();
}
