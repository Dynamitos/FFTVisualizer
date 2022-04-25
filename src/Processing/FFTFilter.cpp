#include "FFTFilter.h"
#include <iostream>

FFTFilter::FFTFilter()
    : lastTimeStamp(0)
{
}

FFTFilter::~FFTFilter()
{
    delete[] targets;
    delete[] values;
}

void FFTFilter::init(ProcessorInitInfo initInfo)
{
    numSamples = initInfo.fftSize;
    numBands = initInfo.numBands;
    tesselationLevel = initInfo.tesselationLevel;
    targets = new float[numBands * tesselationLevel];
    values = new float[numBands * tesselationLevel];
}

void FFTFilter::finalizeConverterInfo(ConverterInitInfo& initInfo)
{
    //Do nothing, we dont need to convert anything
}

void FFTFilter::forward(std::unique_ptr<SampleContainer> container)
{
    std::memset(targets, 0, sizeof(float) * numBands * tesselationLevel);
    std::memset(values, 0, sizeof(float) * numBands * tesselationLevel);
    //initialize the result
    std::unique_ptr<SampleContainer> result = std::make_unique<SampleContainer>();
    uint32_t outputSize = numBands * tesselationLevel * sizeof(float);
    result->convertedSamples = new uint8_t[outputSize];
    result->dataSize = outputSize;
    result->numSamples = numBands;
    result->timeStamp = container->timeStamp;

    //calculate the current time delta
    int64_t timeDelta = container->timeStamp - lastTimeStamp;
    lastTimeStamp = container->timeStamp;

    //cast bands to float, since we know FFT only outputs floats
    uint32_t targetSize = numBands * tesselationLevel;
    float* bands = (float*)container->convertedSamples;


    //initialize variables used for tesselation
    float prevValue, nextValue;
    int offset = tesselationLevel / 2;
    int prevIndex = 0, nextIndex = offset;
    uint32_t bandIndex = 0;
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
    //tone down values to get the more in the [0...1] range 
    for (uint32_t i = 0; i < targetSize; ++i)
    {
        targets[i] /= 1000.f;
    }
    //smooth the edges of the tesselated values
    reverseSmooth(targets, 0.9f);

    //smooth the result with the last used values over time to avoid jumping bands
    for (uint32_t i = 0; i < targetSize; ++i)
    {
        values[i] -= (values[i] - targets[i]) * timeDelta *.00000001f;
    }

    std::memcpy(result->convertedSamples, values, outputSize);
    outputQueue->addToQueue(std::move(result));
}

void FFTFilter::smooth(float* data, float distribution)
{
    for (uint32_t i = 1; i < numBands * tesselationLevel; ++i)
    {
        data[i] = data[i - 1] * distribution + data[i] * (1 - distribution);
    }
}

void FFTFilter::reverseSmooth(float* data, float distribution)
{
    for (int32_t i = (numBands * tesselationLevel) - 2; i >= 0; --i)
    {
        data[i] = data[i + 1] * distribution + data[i] * (1 - distribution);
    }
}
