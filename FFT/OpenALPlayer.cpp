#include "OpenALPlayer.h"

OpenALPlayer::OpenALPlayer()
	: buffers{ nullptr }
	, ctx{ nullptr }
	, dev{ nullptr }
	, format{ 0 }
	, frequency{ 0 }
	, numBuffers{ 0 }
	, source{ 0 }
	, isDoubleSupported{ false }
	, isFloatSupported{ false }
{

}

OpenALPlayer::~OpenALPlayer()
{
	// Free OpenAL resources
	alDeleteSources(1, &source);
	alDeleteBuffers(numBuffers, buffers);

	alcMakeContextCurrent(NULL);
	alcDestroyContext(ctx);
	alcCloseDevice(dev);
	delete buffers;
}

void OpenALPlayer::init(PlayerInitInfo initInfo)
{
	numBuffers = initInfo.numBuffers;
	dev = alcOpenDevice(nullptr);
	if (!dev)
	{
		fprintf(stderr, "Error initializing device\n");
	}
	
	ctx = alcCreateContext(dev, nullptr);
	alcMakeContextCurrent(ctx);

	if (!ctx)
	{
		fprintf(stderr, "Error");
	}
	if (alIsExtensionPresent("AL_EXT_float32"))
	{
		isFloatSupported = true;
		alEnable(AL_EXT_float32);
	}
	if (alIsExtensionPresent("AL_EXT_double"))
	{
		isDoubleSupported = true;
		alEnable(AL_EXT_double);
	}

	int error = alGetError();
	if (error != AL_NO_ERROR)
	{
		fprintf(stderr, "loading extension: %d\n", error);
		return;
	}

	buffers = new ALuint[numBuffers];
	alGenBuffers(numBuffers, buffers);
	alGenSources(1, &source);
	if (alGetError() != AL_NO_ERROR)
	{
		fprintf(stderr, "Error generating\n");
		return;
	}
}

void OpenALPlayer::playSamples(std::unique_ptr<ConvertedSampleContainer> container)
{
	//since the container is passed by reference, it is possible that OpenAL is still waiting with the transfer
	//which means that the container cant be used for new samples in the meantime

	if (initBuffer)
	{
		alBufferData(buffers[bufferNumber], format, container->convertedSamples, container->dataSize, frequency);

		int error = alGetError();
		if (error != AL_NO_ERROR)
		{
			fprintf(stderr, "Error loading: %d\n", error);
			return;
		}

		if (bufferNumber == numBuffers - 1)
		{
			alSourceQueueBuffers(source, numBuffers, buffers);

			alSourcePlay(source);
			if (alGetError() != AL_NO_ERROR)
			{
				fprintf(stderr, "Error starting\n");
				return;
			}
			initBuffer = 0;
		}
		bufferNumber = (bufferNumber + 1) % numBuffers;
	}
	else
	{
		ALuint buffer;
		ALint val;
		do {
			alGetSourcei(source, AL_BUFFERS_PROCESSED, &val);
		} while (val <= 0);

		alSourceUnqueueBuffers(source, 1, &buffer);
		alBufferData(buffer, format, container->convertedSamples, container->dataSize, frequency);
		alSourceQueueBuffers(source, 1, &buffer);
		if (alGetError() != AL_NO_ERROR)
		{
			fprintf(stderr, "Error buffering\n");
			return;
		}

		alGetSourcei(source, AL_SOURCE_STATE, &val);
		if (val != AL_PLAYING)
			alSourcePlay(source);
	}
}

void OpenALPlayer::finalizeConverterInfo(ConverterInitInfo & converterInfo)
{
	frequency = converterInfo.inSampleRate;
	AVSampleFormat inFormat = converterInfo.inSampleFormat;
	AVSampleFormat packedFormat = av_get_packed_sample_fmt(converterInfo.inSampleFormat);
	if (packedFormat != AV_SAMPLE_FMT_NONE)
	{
		inFormat = packedFormat;
		converterInfo.outSampleFormat = packedFormat;
	}
	if (converterInfo.inChannelLayout == AV_CH_LAYOUT_MONO)
	{
		switch (inFormat)
		{
		case AV_SAMPLE_FMT_U8:
			format = AL_FORMAT_MONO8;
			break;
		case AV_SAMPLE_FMT_S16:
			format = AL_FORMAT_MONO16;
			break;
			//OpenAL doesnt support 32 or 64 bit mono audio, convert to 16 bit
		case AV_SAMPLE_FMT_S32:
			format = AL_FORMAT_MONO16;
			converterInfo.outSampleFormat = AV_SAMPLE_FMT_S16;
			break;
		case AV_SAMPLE_FMT_S64:
			format = AL_FORMAT_MONO16;
			converterInfo.outSampleFormat = AV_SAMPLE_FMT_S16;
			break;
		case AV_SAMPLE_FMT_FLT:
			if (!isFloatSupported)
			{
				format = AL_FORMAT_MONO16;
				converterInfo.outSampleFormat = AV_SAMPLE_FMT_S16;
				break;
			}
			else
			{
				format = AL_FORMAT_MONO_FLOAT32;
			}
			break;
		case AV_SAMPLE_FMT_DBL:
			if (!isDoubleSupported)
			{
				format = AL_FORMAT_MONO16;
				converterInfo.outSampleFormat = AV_SAMPLE_FMT_S16;
			}
			else
			{
				format = AL_FORMAT_MONO_DOUBLE_EXT;
			}
			break;
		}
	}
	else if (converterInfo.inChannelLayout == AV_CH_LAYOUT_STEREO)
	{
		switch (inFormat)
		{
		case AV_SAMPLE_FMT_U8:
			format = AL_FORMAT_STEREO8;
			break;
		case AV_SAMPLE_FMT_S16:
			format = AL_FORMAT_STEREO16;
			break;
			//OpenAL doesnt support 32 or 64 bit STEREO audio, convert to 16 bit
		case AV_SAMPLE_FMT_S32:
			format = AL_FORMAT_STEREO16;
			converterInfo.outSampleFormat = AV_SAMPLE_FMT_S16;
			break;
		case AV_SAMPLE_FMT_S64:
			format = AL_FORMAT_STEREO16;
			converterInfo.outSampleFormat = AV_SAMPLE_FMT_S16;
			break;
		case AV_SAMPLE_FMT_FLT:
			if (!isFloatSupported)
			{
				format = AL_FORMAT_STEREO16;
				converterInfo.outSampleFormat = AV_SAMPLE_FMT_S16;
				break;
			}
			else
			{
				format = AL_FORMAT_STEREO_FLOAT32;
			}
			break;
		case AV_SAMPLE_FMT_DBL:
			if (!isDoubleSupported)
			{
				format = AL_FORMAT_STEREO16;
				converterInfo.outSampleFormat = AV_SAMPLE_FMT_S16;
			}
			else
			{
				format = AL_FORMAT_STEREO_DOUBLE_EXT;
			}
			break;
		}
	}
	else if (converterInfo.inChannelLayout == AV_CH_LAYOUT_QUAD) {
		switch (inFormat)
		{
		case AV_SAMPLE_FMT_U8:
			format = AL_FORMAT_QUAD8;
			break;
		case AV_SAMPLE_FMT_S16:
			format = AL_FORMAT_QUAD16;
			break;
			//OpenAL QUAD32 is a float format, convert to float to preserve quality
		case AV_SAMPLE_FMT_S32:
			format = AL_FORMAT_QUAD32;
			converterInfo.outSampleFormat = AV_SAMPLE_FMT_FLT;
			break;
		case AV_SAMPLE_FMT_S64:
			format = AL_FORMAT_QUAD32;
			converterInfo.outSampleFormat = AV_SAMPLE_FMT_FLT;
			break;
		case AV_SAMPLE_FMT_FLT:
			format = AL_FORMAT_QUAD32;
			break;
		case AV_SAMPLE_FMT_DBL:
			format = AL_FORMAT_QUAD32;
			converterInfo.outSampleFormat = AV_SAMPLE_FMT_FLT;
			break;
		default:
			break;
		}
	}
	else if (converterInfo.inChannelLayout == AV_CH_LAYOUT_5POINT1) {
		switch (inFormat)
		{
		case AV_SAMPLE_FMT_U8:
			format = AL_FORMAT_51CHN8;
			break;
		case AV_SAMPLE_FMT_S16:
			format = AL_FORMAT_51CHN16;
			break;
			//OpenAL QUAD32 is a float format, convert to float to preserve quality
		case AV_SAMPLE_FMT_S32:
			format = AL_FORMAT_51CHN32;
			converterInfo.outSampleFormat = AV_SAMPLE_FMT_FLT;
			break;
		case AV_SAMPLE_FMT_S64:
			format = AL_FORMAT_51CHN32;
			converterInfo.outSampleFormat = AV_SAMPLE_FMT_FLT;
			break;
		case AV_SAMPLE_FMT_FLT:
			format = AL_FORMAT_51CHN32;
			break;
		case AV_SAMPLE_FMT_DBL:
			format = AL_FORMAT_51CHN32;
			converterInfo.outSampleFormat = AV_SAMPLE_FMT_FLT;
			break;
		default:
			break;
		}
	}
	else if (converterInfo.inChannelLayout == AV_CH_LAYOUT_6POINT1) {
		switch (inFormat)
		{
		case AV_SAMPLE_FMT_U8:
			format = AL_FORMAT_61CHN8;
			break;
		case AV_SAMPLE_FMT_S16:
			format = AL_FORMAT_61CHN16;
			break;
			//OpenAL QUAD32 is a float format, convert to float to preserve quality
		case AV_SAMPLE_FMT_S32:
			format = AL_FORMAT_61CHN32;
			converterInfo.outSampleFormat = AV_SAMPLE_FMT_FLT;
			break;
		case AV_SAMPLE_FMT_S64:
			format = AL_FORMAT_61CHN32;
			converterInfo.outSampleFormat = AV_SAMPLE_FMT_FLT;
			break;
		case AV_SAMPLE_FMT_FLT:
			format = AL_FORMAT_61CHN32;
			break;
		case AV_SAMPLE_FMT_DBL:
			format = AL_FORMAT_61CHN32;
			converterInfo.outSampleFormat = AV_SAMPLE_FMT_FLT;
			break;
		default:
			break;
		}
	}
	else if (converterInfo.inChannelLayout == AV_CH_LAYOUT_7POINT1) {
		switch (inFormat)
		{
		case AV_SAMPLE_FMT_U8:
			format = AL_FORMAT_71CHN8;
			break;
		case AV_SAMPLE_FMT_S16:
			format = AL_FORMAT_71CHN16;
			break;
			//OpenAL QUAD32 is a float format, convert to float to preserve quality
		case AV_SAMPLE_FMT_S32:
			format = AL_FORMAT_71CHN32;
			converterInfo.outSampleFormat = AV_SAMPLE_FMT_FLT;
			break;
		case AV_SAMPLE_FMT_S64:
			format = AL_FORMAT_71CHN32;
			converterInfo.outSampleFormat = AV_SAMPLE_FMT_FLT;
			break;
		case AV_SAMPLE_FMT_FLT:
			format = AL_FORMAT_71CHN32;
			break;
		case AV_SAMPLE_FMT_DBL:
			format = AL_FORMAT_71CHN32;
			converterInfo.outSampleFormat = AV_SAMPLE_FMT_FLT;
			break;
		default:
			break;
		}
	}
}
