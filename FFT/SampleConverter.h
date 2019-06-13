#pragma once
#include "Resources.h"
#include "PacketQueue.h"
#include <vector>
extern "C"
{
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
}
class SampleConverter
{
public:
	SampleConverter(ConverterInitInfo converterInfo);
	virtual ~SampleConverter();

	void run(PacketQueue<AudioInputContainer>* inputQueue, PacketQueue<SampleContainer>* outputQueue);

	std::unique_ptr<SampleContainer> convertFrame(std::unique_ptr<AudioInputContainer> audioSamples);
	void convertFrame(AVFrame* inFrame, std::vector<uint8_t>& outputBuffer, int& dataSize);
private:
	SwrContext* swrContext;
	//retain for later
	ConverterInitInfo initInfo;

};