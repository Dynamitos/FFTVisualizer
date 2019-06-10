#pragma once
#include "Resources.h"
#include "AudioPlayer.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

class OpenALPlayer : public AudioPlayer
{
public:
	OpenALPlayer();
	~OpenALPlayer();
	virtual void init(PlayerInitInfo initInfo);
	virtual void playSamples(std::unique_ptr<ConvertedSampleContainer> container);
	virtual void finalizeConverterInfo(ConverterInitInfo& converterInfo);
private:
	ALCdevice* dev;
	ALCcontext* ctx;


	int initBuffer = 1;
	int bufferNumber = 0;
	int numBuffers;
	ALuint source, *buffers;
	ALuint frequency;
	ALuint format;
	bool isFloatSupported;
	bool isDoubleSupported;
};