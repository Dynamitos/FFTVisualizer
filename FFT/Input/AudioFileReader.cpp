#include "AudioFileReader.h"

AudioFileReader::AudioFileReader()
	: codecContext{ nullptr }
	, formatContext{ 0 }
	, frame{ nullptr }
	, packet{ nullptr }
	, streamIndex{ 0 }
	, currentBufferIndex{ 0 }
{
}

AudioFileReader::~AudioFileReader()
{
	avcodec_close(codecContext);
	avcodec_free_context(&codecContext);

	avformat_free_context(formatContext);
}

void AudioFileReader::setStreamSource(AudioSourceInfo& info)
{
	formatContext = avformat_alloc_context();

	avformat_open_input(&formatContext, info.url, nullptr, nullptr);
	avformat_find_stream_info(formatContext, nullptr);

	AVCodecParameters* localCodecParameters = nullptr;
	AVCodec* localCodec = nullptr;

	for (uint32_t i = 0; i < formatContext->nb_streams; i++)
	{
		localCodecParameters = formatContext->streams[i]->codecpar;

		localCodec = avcodec_find_decoder(localCodecParameters->codec_id);

		if (localCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			streamIndex = i;

			printf("Audio Codec: %d channels, sample rate %d\n", localCodecParameters->channels, localCodecParameters->sample_rate);
			break;
		}
	}

	codecContext = avcodec_alloc_context3(localCodec);
	avcodec_parameters_to_context(codecContext, localCodecParameters);
	avcodec_open2(codecContext, localCodec, nullptr);

	packet = av_packet_alloc();
	frame = av_frame_alloc();
}

std::unique_ptr<AudioInputContainer> AudioFileReader::readSamples()
{
	while (true)
	{
		int result = av_read_frame(formatContext, packet);
		if (result != 0)
		{
			packet->size = 0;
			packet->data = nullptr;

			avcodec_send_packet(codecContext, packet);
			return nullptr;
		}

		if (packet->stream_index == streamIndex)
		{
			break;
		}
	}
	std::unique_ptr<AudioInputContainer> sampleContainer = std::make_unique<AudioInputContainer>();
	avcodec_send_packet(codecContext, packet);
	avcodec_receive_frame(codecContext, frame);
	uint32_t audioByteSize = frame->nb_samples * av_get_bytes_per_sample(codecContext->sample_fmt);
	if (sampleContainer->allocatedChannels < frame->channels)
	{
		if (sampleContainer->samples != nullptr)
		{
			//already allocated, free existing resources
			for (uint32_t i = 0; i < sampleContainer->allocatedChannels; ++i)
			{
				//delete channel data
				delete sampleContainer->samples[i];
			}
			delete sampleContainer->samples;
		}
		//create new channel array, but no buffers yet
		sampleContainer->samples = new uint8_t * [frame->channels];
		for (uint32_t i = 0; i < frame->channels; ++i)
		{
			//we do this so that the new channels are recognized as empty and not accidently deleted
			sampleContainer->samples[i] = nullptr;
		}
		sampleContainer->allocatedChannels = frame->channels;
	}
	if (sampleContainer->allocatedData < audioByteSize)
	{
		//channels allocated, but too small
		for (uint32_t i = 0; i < sampleContainer->allocatedChannels; ++i)
		{
			//delete channel data, but not the channel array
			if (sampleContainer->samples[i] != nullptr) delete sampleContainer->samples[i];
		}
		//create new buffers in existing channel array
		for (uint32_t i = 0; i < frame->channels; ++i)
		{
			//for each channel allocate new buffer
			sampleContainer->samples[i] = new uint8_t[audioByteSize];
		}
		sampleContainer->allocatedData = audioByteSize;
	}
	//we now have guaranteed that we have enough channels and enough space in each channel
	for (uint32_t i = 0; i < frame->channels; ++i)
	{
		//copy each channel individually
		std::memcpy(sampleContainer->samples[i], frame->extended_data[i], audioByteSize);
	}
	sampleContainer->dataSize = audioByteSize;
	sampleContainer->numChannels = frame->channels;
	sampleContainer->numSamples = frame->nb_samples;
	sampleContainer->timeStamp = av_q2d(codecContext->time_base) * frame->pts;

	return sampleContainer;
}

uint32_t AudioFileReader::getSampleRate()
{
	return static_cast<uint32_t>(codecContext->sample_rate);
}

void AudioFileReader::fillConverterInfo(ConverterInitInfo & converterInfo)
{
	converterInfo.inChannelCount = codecContext->channels;
	converterInfo.inChannelLayout = codecContext->channel_layout;
	converterInfo.inSampleFormat = codecContext->sample_fmt;
	converterInfo.inSampleRate = codecContext->sample_rate;

	converterInfo.outChannelCount = codecContext->channels;
	converterInfo.outChannelLayout = codecContext->channel_layout;
	converterInfo.outSampleFormat = codecContext->sample_fmt;
	converterInfo.outSampleRate = codecContext->sample_rate;
}
