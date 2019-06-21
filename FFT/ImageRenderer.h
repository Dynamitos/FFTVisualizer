#pragma once
#include "Resources.h"
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
		RawModel* rawModel;
		glm::mat4 transformationMatrix;
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
		ImageShader* shader;
		bool isScaling;
		GLint textureID;
		Camera* camera;
		float vertices[12]{ -1.f, -1.f, -1.f, 1.f, -1.f, -1.f, -1.f, 1.f, -1.f, 1.f, 1.f, -1.f, };
		float texCoords[8]{ 0, 1, 1, 1, 0, 0, 1, 0 };
		const float FOV = 70.f;
		const float NEAR_PLANE = 1.f;
		const float FAR_PLANE = 1000.f;

	};
}