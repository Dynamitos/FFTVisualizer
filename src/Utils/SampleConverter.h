#pragma once
#include "Resources.h"
#include "PacketQueue.h"

class SampleConverter
{
public:
	SampleConverter(ConverterInitInfo converterInfo);
	virtual ~SampleConverter();

	void run(PacketQueue<AudioInputContainer>* inputQueue, PacketQueue<SampleContainer>* outputQueue);

	std::unique_ptr<SampleContainer> convertFrame(std::unique_ptr<AudioInputContainer> audioSamples);
private:
	struct SwrContext* swrContext;
	//retain for later
	ConverterInitInfo initInfo;

};