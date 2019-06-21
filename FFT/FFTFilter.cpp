#include "FFTFilter.h"

FFTFilter::FFTFilter()
{
}

void FFTFilter::init(ProcessorInitInfo initInfo)
{
	numSamples = initInfo.fftSize;
	numBands = initInfo.numBands;
	tesselationLevel = initInfo.tesselationLevel;
}

void FFTFilter::finalizeConverterInfo(ConverterInitInfo& initInfo)
{
	//Do nothing, we dont need to convert anything
}

void FFTFilter::forward(std::unique_ptr<SampleContainer> container)
{
	std::unique_ptr<SampleContainer> result = std::make_unique<SampleContainer>();
	uint32_t outputSize = numBands * tesselationLevel * sizeof(float);
	result->convertedSamples = new uint8_t[outputSize];
	result->dataSize = outputSize;
	result->numSamples = numBands;
	result->timeStamp = container->timeStamp;

	uint32_t targetSize = numBands * numSamples;
	float* bands = (float*)container->convertedSamples;
	float* targets = new float[targetSize];
	float prevValue, nextValue;
	int offset = tesselationLevel / 2;
	int prevIndex = 0, nextIndex = offset, bandIndex = 0;
	prevValue = bands[bandIndex];
	nextValue = bands[++bandIndex];
	for (uint32_t i = 0; i < targetSize; ++i)
	{
		float weight = (1.f * i - prevIndex) / (nextIndex - prevIndex);
		targets[i] = prevValue * (1 - weight) + nextValue * weight;
		if (i == nextIndex)
		{
			prevValue = nextValue;
			nextValue = (++bandIndex < numBands) ? bands[bandIndex] : 0;
			prevIndex = nextIndex;
			nextIndex += tesselationLevel;
		}
	}
	reverseSmooth(targets, 0.9f);
	std::memcpy(result->convertedSamples, targets, outputSize);
	delete[] targets;
	outputQueue->addToQueue(std::move(result));
}

void FFTFilter::smooth(float* data, float distribution)
{
	for (int32_t i = 1; i < numBands * numSamples; ++i)
	{
		data[i] = data[i - 1] * distribution + data[i] * (1 - distribution);
	}
}

void FFTFilter::reverseSmooth(float* data, float distribution)
{
	for (int32_t i = (numBands * numSamples) - 2; i >= 0; --i)
	{
		data[i] = data[i + 1] * distribution + data[i] * (1 - distribution);
	}
}
