#pragma once
#include "PostProcessingShader.h"

class Display;
namespace GL
{
	class Loader;
	class PostProcessingRenderer
	{
	public:
		PostProcessingRenderer();
		void init(Loader* loader, Display* display, AudioVisualizerInfo info);
		void beginMainPass();
		void render();
		void unbindCurrentFramebuffer();
		void bindFramebuffer(GLuint framebuffer, int width, int height);
	private:
		GLuint createFramebuffer();
		GLuint createTextureAttachment(int width, int height);
		GLuint mainPassBuffer;
		GLuint mainPassTexture;
		PostProcessingShader* shader;
		float currentTime;
		GLuint overlayID;
		GLuint textID;
		AudioVisualizerInfo info;
		Display* display;
	};
}