#include "Visualizer.h"
#include "Loader.h"

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
}

void Renderer::renderData(std::unique_ptr<SampleContainer> data)
{
	float* fftData = (float*)data->convertedSamples;
	imageRenderer->render(calcBass(fftData, data->numSamples));
	display->updateDisplay();
}

float GL::Renderer::calcBass(float* bands, int numBands)
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
