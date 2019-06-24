#pragma once
#include "AudioProcessor.h"

class FFTFilter : public AudioProcessor
{
public:
	FFTFilter();
	~FFTFilter();
	// Inherited via AudioProcessor
	virtual void init(ProcessorInitInfo initInfo) override;
	virtual void finalizeConverterInfo(ConverterInitInfo& initInfo) override;
	virtual void forward(std::unique_ptr<SampleContainer> container) override;
private: 
	void smooth(float* data, float distribution);
	void reverseSmooth(float* data, float distribution);

	uint32_t numBands;
	uint32_t numSamples;
	uint32_t tesselationLevel;
	int64_t lastTimeStamp;
	float bassGain;
	float rawBassGain;
	float* values;
	float* targets;
};
