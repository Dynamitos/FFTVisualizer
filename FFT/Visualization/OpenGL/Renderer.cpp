#include "Renderer.h"
#include "Loader.h"
#include "ImageRenderer.h"
#include "LineRenderer.h"
#include "PostProcessingRenderer.h"

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
	imageRenderer = new ImageRenderer();
	imageRenderer->init(loader, visualizerInfo);
	lineRenderer = new LineRenderer();
	lineRenderer->init(visualizerInfo);
	postProcessor = new PostProcessingRenderer();
	postProcessor->init(loader, display, visualizerInfo);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glLineWidth(2.f);
}

void Renderer::renderData(std::unique_ptr<SampleContainer> data)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	float* fftData = (float*)data->convertedSamples;
	postProcessor->beginMainPass();
	imageRenderer->render(calcBass(fftData, data->numSamples));
	lineRenderer->render(fftData);
	postProcessor->unbindCurrentFramebuffer();
	postProcessor->render();
	display->updateDisplay();
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
		bassGain -= 0.1f * display->getCurrentFrameTime();
	else
		bassGain = tempGain;
	return bassGain;
}
