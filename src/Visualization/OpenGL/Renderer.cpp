#include "Renderer.h"
#include "Loader.h"
#include "ImageRenderer.h"
#include "LineRenderer.h"
#include "PostProcessingRenderer.h"
#include "ParticleRenderer.h"

using namespace GL;

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::init(AudioVisualizerInfo visualizerInfo)
{
	display = new Display();
	display->createDisplay(visualizerInfo);
	display->showWindow();
	loader = new Loader();
	GL_CHECK();
	imageRenderer = new ImageRenderer();
	imageRenderer->init(loader, visualizerInfo);
	GL_CHECK();
	lineRenderer = new LineRenderer();
	lineRenderer->init(visualizerInfo);
	GL_CHECK();
	particleRenderer = new ParticleRenderer();
	particleRenderer->init(loader, display, visualizerInfo);
	GL_CHECK();
	postProcessor = new PostProcessingRenderer();
	postProcessor->init(loader, display, visualizerInfo);
	GL_CHECK();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glLineWidth(2.f);
}

void Renderer::renderData(std::unique_ptr<SampleContainer> data)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GL_CHECK();
	float* fftData = (float*)data->convertedSamples;
	postProcessor->beginMainPass();
	GL_CHECK();
	float bass = calcBass(fftData, data->numSamples);
	imageRenderer->render(bass);
	GL_CHECK();
	lineRenderer->render(fftData);
	GL_CHECK();
	particleRenderer->render(bass);
	GL_CHECK();
	postProcessor->unbindCurrentFramebuffer();
	GL_CHECK();
	postProcessor->render();
	GL_CHECK();
	display->updateDisplay();
	GL_CHECK();
}

bool Renderer::shouldClose()
{
	return display->shouldClose();
}

float Renderer::calcBass(float* bands, int numBands)
{
	float tempGain = 0;
	for (int i = 0; i < numBands; ++i)
	{
		tempGain += bands[i] / (1 + i * i);
	}
	float rawBassGain = tempGain;
	if (tempGain < bassGain)
		bassGain -= 0.1f * (float)display->getCurrentFrameTime();
	else
		bassGain = tempGain;
	return bassGain;
}
