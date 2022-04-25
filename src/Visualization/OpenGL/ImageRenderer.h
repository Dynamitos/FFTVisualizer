#pragma once
#include "ImageShader.h"
namespace GL
{
	class Camera;
	class Loader;

	class ImageRenderer
	{
	public:
		ImageRenderer();
		void init(Loader* loader, AudioVisualizerInfo visualizerInfo);
		void render(float bass);

	private:
		glm::mat4 transformationMatrix;
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
		ImageShader* shader;
		bool isScaling;
		GLuint quadID;
		GLint textureID;
		Camera* camera;
		const float FOV = 70.f;
		const float NEAR_PLANE = 1.f;
		const float FAR_PLANE = 1000.f;

	};
}