#include "ParticleRenderer.h"
#include "Loader.h"
#include "Display.h"
#include <time.h>

using namespace GL;

ParticleRenderer::ParticleRenderer()
{
}

void ParticleRenderer::init(Loader* loader, Display* display, AudioVisualizerInfo info)
{
	shader = new ParticleShader();
	position = glm::vec3(0, 0, -10);
	center = glm::vec3(0, 0, 0);
	up = glm::vec3(0, 1, 0);
	viewMatrix = glm::lookAt(position, center, up);

	rawModel = loader->loadToVAO(vertices, sizeof(vertices), 2);

	glBindVertexArray(rawModel->vaoID);
	glGenBuffers(1, &vboParticle);
	glBindBuffer(GL_ARRAY_BUFFER, vboParticle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particles), particles, GL_STREAM_DRAW);

	glVertexAttribPointer(1, 4, GL_FLOAT, false, VERTEX_SIZE, (void*)offsetof(Particle, positionScale));
	glVertexAttribPointer(2, 4, GL_FLOAT, false, VERTEX_SIZE, (void*)offsetof(Particle, rotationWeight));
	glVertexAttribPointer(3, 4, GL_FLOAT, false, VERTEX_SIZE, (void*)offsetof(Particle, speedLife));
	glVertexAttribPointer(4, 2, GL_FLOAT, false, VERTEX_SIZE, (void*)offsetof(Particle, dimensions));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	projectionMatrix = glm::perspective(FOV, info.screenDimensions.x / (float)info.screenDimensions.y, NEAR_PLANE, FAR_PLANE);
	shader->start();
	shader->loadProjectionMatrix(projectionMatrix);
	shader->loadTexture();
	shader->stop();
	this->loader = loader;
	this->display = display;
	atlasTexture = loader->loadTexture("particle.png");
	std::memset(particles, 0, sizeof(particles));
	srand(time(NULL));
}

int ParticleRenderer::findUnusedParticle()
{
	for (int i = lastUsedParticle; i < MAX_PARTICLES; ++i)
	{
		if (particles[i].speedLife.w <= 0)
		{
			lastUsedParticle = i;
			return i;
		}
	}
	for (int i = 0; i < lastUsedParticle; ++i)
	{
		if (particles[i].speedLife.w <= 0)
		{
			lastUsedParticle = i;
			return i;
		}
	}
	return 0;
}
static float random(float min, float max)
{
	assert(max > min);
	float seed = rand() / (float)RAND_MAX;
	float interval = max - min;
	return (seed * interval) + min;
}

void ParticleRenderer::render(float intensity)
{
	shader->start();
	
	float frameTime = display->getCurrentFrameTime();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	shader->loadViewMatrix(viewMatrix);
	counter += frameTime * 10000 * intensity;

	int numParticles = (int)counter;
	counter -= numParticles;

	for (int i = 0; i < numParticles; ++i)
	{
		int index = findUnusedParticle();
		Particle& p = particles[index];
		p.positionScale = glm::vec4(random(-5, 5), -10, random(-5, 5), 0.1f);
		p.speedLife = glm::vec4(random(-0.5f, 0.5f), 2, random(-0.5f, 0.5f), 10.f);
		p.rotationWeight = glm::vec4(0, 0, 0, 0);
		p.dimensions = glm::vec2(1, 1);
	}
}
