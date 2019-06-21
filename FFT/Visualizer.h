#pragma once
#include "Resources.h"
#include "AudioVisualizer.h"
#include "ImageRenderer.h"
#include "LineRenderer.h"
#include "Display.h"
namespace GL
{
	class Renderer : public AudioVisualizer
	{
	public:
		Renderer();
		virtual ~Renderer();

		virtual void init(AudioVisualizerInfo visualizerInfo);
		virtual void renderData(std::unique_ptr<SampleContainer> data);
	private:
		float calcBass(float* bands, int numBands);
		float bassGain;
		ImageRenderer* imageRenderer;
		LineRenderer* lineRenderer;
		Loader* loader;
		Display* display;
	};
}