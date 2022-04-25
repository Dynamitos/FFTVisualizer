#pragma once
#include "ParticleShader.h"
#include "Visualization\RandomNoise.h"

class Display;
namespace GL
{
	class Loader;
	struct Particle
	{
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 speed;
		glm::vec2 dimensions;
		float scale;
		float weight;
		float life;
	};
	struct PackedParticle
	{
		glm::vec4 positionScale;
		glm::vec4 rotation;
	};
#define MAX_PARTICLES 2 << 16
	class ParticleRenderer
	{
	public:
		ParticleRenderer();
		void init(Loader* loader, Display* display, AudioVisualizerInfo info);
		int findUnusedParticle();
		void render(float intensity);

	private:
		RandomNoise perlinNoise;
		float counter;
		int lastUsedParticle = 0;
		static const int VERTEX_SIZE = sizeof(PackedParticle);
		Particle particles[MAX_PARTICLES];
		PackedParticle particleData[MAX_PARTICLES];
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