#include "PostProcessingRenderer.h"
#include "Loader.h"
#include "Display.h"

using namespace GL;

PostProcessingRenderer::PostProcessingRenderer()
{
}

void PostProcessingRenderer::init(Loader* loader, Display* display, AudioVisualizerInfo info)
{
	this->info = info;
	this->display = display;
	mainPassBuffer = createFramebuffer();
	mainPassTexture = createTextureAttachment(info.screenDimensions.x, info.screenDimensions.y);
	if (info.overlayURL == nullptr)
	{
		uint32_t temp[4]{ 0, 0, 0, 1 };
		overlayID = loader->loadTexture(temp, 4 * sizeof(uint32_t), 1);
	}
	else
	{
		overlayID = loader->loadTexture(info.overlayURL);
	}
	shader = new PostProcessingShader();
}

void PostProcessingRenderer::beginMainPass()
{
	bindFramebuffer(mainPassBuffer, info.screenDimensions.x, info.screenDimensions.y);
}

void PostProcessingRenderer::render()
{
	shader->start();
	shader->loadTexture();
	currentTime += display->getCurrentFrameTime();
	shader->loadTime(currentTime);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mainPassTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, overlayID);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textID);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	shader->stop();
}

void GL::PostProcessingRenderer::unbindCurrentFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, info.screenDimensions.x, info.screenDimensions.y);
}

void GL::PostProcessingRenderer::bindFramebuffer(GLuint framebuffer, int width, int height)
{
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glViewport(0, 0, width, height);
}

GLuint PostProcessingRenderer::createFramebuffer()
{
	GLuint frameBuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	return frameBuffer;
}

GLuint PostProcessingRenderer::createTextureAttachment(int width, int height)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
	return texture;
}
