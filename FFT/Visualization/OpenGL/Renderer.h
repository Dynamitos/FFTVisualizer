#pragma once
#include "Visualization/AudioVisualizer.h"
#include "Display.h"

namespace GL
{
	class ImageRenderer;
	class LineRenderer;
	class Loader;
	class Renderer : public AudioVisualizer
	{
	public:
		Renderer();
		virtual ~Renderer();

		virtual void init(AudioVisualizerInfo visualizerInfo);
		virtual void renderData(std::unique_ptr<SampleContainer> data);
		virtual bool shouldClose();
	private:
		float calcBass(float* bands, int numBands);
		float bassGain;
		ImageRenderer* imageRenderer;
		LineRenderer* lineRenderer;
		Loader* loader;
		Display* display;
	};
}