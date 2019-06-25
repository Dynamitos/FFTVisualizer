#include "ParticleRenderer.h"
#include "Loader.h"
#include "Display.h"
#include <time.h>
#include <thread>

using namespace GL;

ParticleRenderer::ParticleRenderer()
	: perlinNoise(20)
{
}

void ParticleRenderer::init(Loader* loader, Display* display, AudioVisualizerInfo info)
{
	shader = new ParticleShader();
	position = glm::vec3(0, 0, -10);
	center = glm::vec3(0, 0, 0);
	up = glm::vec3(0, 1, 0);
	viewMatrix = glm::lookAt(position, center, up);
	projectionMatrix = glm::perspective(FOV, info.screenDimensions.x / (float)info.screenDimensions.y, NEAR_PLANE, FAR_PLANE);


	rawModel = loader->loadToVAO(vertices, sizeof(vertices), 2);

	glBindVertexArray(rawModel->vaoID);
	glGenBuffers(1, &vboParticle);
	glBindBuffer(GL_ARRAY_BUFFER, vboParticle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particles), particles, GL_STREAM_DRAW);

	glVertexAttribPointer(1, 4, GL_FLOAT, false, VERTEX_SIZE, (void*)offsetof(PackedParticle, positionScale));
	glVertexAttribPointer(2, 4, GL_FLOAT, false, VERTEX_SIZE, (void*)offsetof(PackedParticle, rotation));
	glVertexAttribPointer(3, 2, GL_FLOAT, false, VERTEX_SIZE, (void*)offsetof(PackedParticle, dimensions));
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	shader->start();
	shader->loadProjectionMatrix(projectionMatrix);
	shader->loadViewMatrix(viewMatrix);
	shader->loadTexture();
	shader->stop();
	this->loader = loader;
	this->display = display;
	atlasTexture = loader->loadTexture("particle.png");
	std::memset(particles, 0, sizeof(particles));
	std::memset(particleData, 0, sizeof(particleData));
	srand(time(NULL));
}


int ParticleRenderer::findUnusedParticle()
{
	for (int i = lastUsedParticle; i < MAX_PARTICLES; ++i)
	{
		if (particles[i].life <= 0)
		{
			lastUsedParticle = i;
			return i;
		}
	}
	for (int i = 0; i < lastUsedParticle; ++i)
	{
		if (particles[i].life <= 0)
		{
			lastUsedParticle = i;
			return i;
		}
	}
	return 0;
}

void ParticleRenderer::render(float intensity)
{
	shader->start();
	float frameTime = display->getCurrentFrameTime();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	counter += frameTime * 1000 * intensity;

	int numParticles = (int)counter;
	counter -= numParticles;

	for (int i = 0; i < numParticles; ++i)
	{
		int index = findUnusedParticle();
		Particle& p = particles[index];
		p.position = glm::vec3(random(-5, 5), -10, random(-5, 5));
		p.speed = glm::vec3(random(-0.5f, 0.5f), 2, random(-0.5f, 0.5f));
		p.rotation = glm::vec3(0, 0, 0);
		p.dimensions = glm::vec2(1, 1);
		p.life = 10.f;
		p.scale = 0.5f;
		p.weight = 0.1f;
	}

	int length = 0;
	for (int i = 0; i < MAX_PARTICLES; ++i)
	{
		Particle& p = particles[i];
		if (p.life > 0)
		{
			p.position = p.position + (p.speed * frameTime * 10.f);
			//p.rotation += p.speed * frameTime;

			//p.speed += perlinNoise.perlin(p.position) * 10.f * frameTime;
			p.life -= frameTime;

			particleData[length].positionScale = glm::vec4(p.position, p.scale);
			particleData[length].rotation = glm::vec4();
			particleData[length].dimensions = glm::vec2(p.dimensions);

			length++;
		}
	}

	glBindVertexArray(rawModel->vaoID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, atlasTexture);

	glBindBuffer(GL_ARRAY_BUFFER, vboParticle);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(PackedParticle) * length, particleData);
	
	glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, length);

	printf("NumParticles: %f\n", frameTime);

	glBindVertexArray(0);
	shader->stop();
	glDisable(GL_BLEND);
}
