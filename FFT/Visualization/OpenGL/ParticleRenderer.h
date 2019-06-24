#pragma once
#include "ParticleShader.h"

class Display;
namespace GL
{
	class Loader;
	struct Particle
	{
		glm::vec4 positionScale;
		glm::vec4 rotationWeight;
		glm::vec4 speedLife;	
		glm::vec2 dimensions;
	};
#define MAX_PARTICLES 2048
	class ParticleRenderer
	{
	public:
		ParticleRenderer();
		void init(Loader* loader, Display* display, AudioVisualizerInfo info);
		int findUnusedParticle();
		void render(float intensity);

	private:
		float counter;
		int lastUsedParticle = 0;
		static const int VERTEX_SIZE = sizeof(Particle);
		Particle particles[MAX_PARTICLES];
		GLuint vboParticle;
		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix;
		glm::vec3 position;
		glm::vec3 center;
		glm::vec3 up;
		glm::vec3 attractor;

		float FOV = 70;
		float NEAR_PLANE = 0.1f;
		float FAR_PLANE = 10000.f;

		Loader* loader;
		Display* display;
		int atlasTexture;
		float vertices[8]{ -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f };
		RawModel* rawModel;
		ParticleShader* shader;
	};
}