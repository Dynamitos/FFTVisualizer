#pragma once
#include "AudioInput.h"
extern "C"
{
#include <libavformat/avformat.h>
}

class AudioFileReader : public AudioInput
{
public:
	AudioFileReader();
	virtual ~AudioFileReader();

	virtual void setStreamSource(AudioSourceInfo& info);
	virtual std::unique_ptr<AudioInputContainer> readSamples();
	virtual uint32_t getSampleRate();
	virtual void fillConverterInfo(ConverterInitInfo& converterInfo);
private:
	AVFormatContext* formatContext;
	uint32_t currentBufferIndex;
	AVCodecContext* codecContext;
	int streamIndex;
	int64_t lastTimeStamp;

	AVPacket* packet;
	AVFrame* frame;
};