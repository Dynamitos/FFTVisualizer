extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswresample/swresample.h>
}
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <Audioclient.h>
#include <audiopolicy.h>
#include <mmdeviceapi.h>

#include "sdlutil.h"
#include "FFT.h"


struct AudioParams
{
	int sampleRate;
	int channelLayout;
	int channels;
	AVSampleFormat sampleFormat;
};

void configureSwr(SwrContext* swrContext, AVCodecParameters* codecParams, AudioParams params)
{
	std::cout << "sample rate: " << codecParams->sample_rate << std::endl
		<< "channel layout " << codecParams->channel_layout << std::endl
		<< "channels " << codecParams->channels << std::endl
		<< "sample format " << codecParams->format << std::endl;

	av_opt_set_int(swrContext, "in_sample_rate", codecParams->sample_rate, 0);
	av_opt_set_int(swrContext, "out_sample_rate", params.sampleRate, 0);

	av_opt_set_int(swrContext, "in_channel_count", codecParams->channels, 0);
	av_opt_set_int(swrContext, "out_channel_count", params.channels, 0);

	av_opt_set_channel_layout(swrContext, "in_channel_layout", codecParams->channel_layout, 0);
	av_opt_set_channel_layout(swrContext, "out_channel_layout", params.channelLayout, 0);

	av_opt_set_sample_fmt(swrContext, "in_sample_fmt", (AVSampleFormat)codecParams->format, 0);
	av_opt_set_sample_fmt(swrContext, "out_sample_fmt", params.sampleFormat, 0);

	int ret = swr_init(swrContext);
	if (ret < 0)
	{
		printf("Exiting: %d", ret);
	}
}

#undef main
int main()
{
	Display display(1280, 720);

	AVFormatContext* formatContext = avformat_alloc_context();

	avformat_open_input(&formatContext, "Obsessed.mp3", nullptr, nullptr);

	printf("Format: %s, duration: %lld us\n", formatContext->iformat->long_name, formatContext->duration);

	avformat_find_stream_info(formatContext, nullptr);

	ALCdevice* dev;
	ALCcontext* ctx;
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

	int initBuffer = 1;
	int bufferNumber = 0;
#define NUM_BUFFERS 64
	ALuint source, buffers[NUM_BUFFERS];
	ALuint frequency;
	ALuint format;
	SwrContext* swrContext;
	int stream_index;
	AVCodecParameters* localCodecParameters = nullptr;
	AVCodec* localCodec = nullptr;

	for (int i = 0; i < formatContext->nb_streams; i++)
	{
		localCodecParameters = formatContext->streams[i]->codecpar;

		localCodec = avcodec_find_decoder(localCodecParameters->codec_id);

		if (localCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			stream_index = i;
			frequency = localCodecParameters->sample_rate;
			format = AL_FORMAT_STEREO16;

			printf("Audio Codec: %d channels, sample rate %d\n", localCodecParameters->channels, localCodecParameters->sample_rate);
			break;
		}
	}
#define FFT_SIZE 4096
	FFT* fft = new FFT(FFT_SIZE, frequency);

	swrContext = swr_alloc();

	AudioParams audioParams;
	audioParams.sampleRate = localCodecParameters->sample_rate;
	audioParams.sampleFormat = AV_SAMPLE_FMT_S16;
	audioParams.channelLayout = localCodecParameters->channel_layout;
	audioParams.channels = localCodecParameters->channels;

	configureSwr(swrContext, localCodecParameters, audioParams);

	//Buffer used to store the byte-converted audio samples for OpenAL to play from
	std::vector<uint8_t> outputBuffer;
	//Points to outputBuffer.data(), here for convenience and typecasting
	uint8_t* outputPointer;

	//Buffer used to buffer audio inputs until there are enough for a FFT run
	std::vector<float> fftInputBuffer(FFT_SIZE * 4);
	//Index to mark the current head of the buffer. Functionally equivalent to fftInputBuffer.size(), but used instead to avoid reallocating a vector
	int fftBufferIndex = 0;

	std::vector<float> fftOutputBuffer(FFT_SIZE);

	alGenBuffers(NUM_BUFFERS, buffers);
	alGenSources(1, &source);
	if (alGetError() != AL_NO_ERROR)
	{
		fprintf(stderr, "Error generating\n");
		return 1;
	}

	AVCodecContext* codecContext = avcodec_alloc_context3(localCodec);
	avcodec_parameters_to_context(codecContext, localCodecParameters);
	avcodec_open2(codecContext, localCodec, nullptr);

	AVPacket* packet = av_packet_alloc();
	AVFrame* frame = av_frame_alloc();

	while (av_read_frame(formatContext, packet) >= 0)
	{
		if (packet->stream_index != stream_index)
		{
			continue;
		}

		avcodec_send_packet(codecContext, packet);
		avcodec_receive_frame(codecContext, frame);


		float* inputBufferPointer = &fftInputBuffer[fftBufferIndex];
		std::memcpy(inputBufferPointer, frame->data[0]/*use index 0 for first channel*/, frame->linesize[0]);
		fftBufferIndex += frame->nb_samples;
		if (fftBufferIndex >= FFT_SIZE)
		{
			//The buffer is now filled with more than FFT_SIZE samples, perform FFT on samples 0-FFT_SIZE
			fft->forward(fftInputBuffer.data(), FFT_SIZE);
			for (int i = 0; i < 64; ++i)
			{
				fftOutputBuffer[i] = fft->getBand(i);
			}
			display.drawFFT(fftOutputBuffer.data(), 64);

			//Now reset buffer
			//copy samples that weren't processed to the beginning of the buffer
			fftBufferIndex -= FFT_SIZE;
			std::memcpy(fftInputBuffer.data(), fftInputBuffer.data() + FFT_SIZE, fftBufferIndex * sizeof(float));
		}

		int maxOutput = swr_get_out_samples(swrContext, frame->nb_samples);
		if (outputBuffer.size() < maxOutput)
		{
			outputBuffer.resize(maxOutput * 4);
			outputPointer = outputBuffer.data();
			std::cout << "Resizing output buffer to " << maxOutput << std::endl;
		}
		int ret = swr_convert(swrContext, &outputPointer, frame->nb_samples, (const uint8_t * *)frame->extended_data, frame->nb_samples);
		//std::cout << "Convert returned: " << ret << std::endl;
		if (ret < 0) {
			fprintf(stderr, "Error while converting\n");
			return 1;
		}

		int data_size = av_samples_get_buffer_size(nullptr, localCodecParameters->channels, frame->nb_samples, AV_SAMPLE_FMT_S16, 1);

		if (initBuffer)
		{
			alBufferData(buffers[bufferNumber], format, outputPointer, data_size, frequency);

			int error = alGetError();
			if (error != AL_NO_ERROR)
			{
				fprintf(stderr, "Error loading: %d\n", error);
				return 1;
			}

			if (bufferNumber == NUM_BUFFERS - 1)
			{
				alSourceQueueBuffers(source, NUM_BUFFERS, buffers);

				alSourcePlay(source);
				if (alGetError() != AL_NO_ERROR)
				{
					fprintf(stderr, "Error starting\n");
					return 1;
				}
				initBuffer = 0;
			}
			bufferNumber = (bufferNumber + 1) % NUM_BUFFERS;
		}
		else
		{
			ALuint buffer;
			ALint val;
			do {
				alGetSourcei(source, AL_BUFFERS_PROCESSED, &val);
			} while (val <= 0);

			alSourceUnqueueBuffers(source, 1, &buffer);
			alBufferData(buffer, format, outputPointer, data_size, frequency);
			alSourceQueueBuffers(source, 1, &buffer);
			if (alGetError() != AL_NO_ERROR)
			{
				fprintf(stderr, "Error buffering\n");
				return 1;
			}

			alGetSourcei(source, AL_SOURCE_STATE, &val);
			if (val != AL_PLAYING)
				alSourcePlay(source);
		}

	}

	packet->size = 0;
	packet->data = nullptr;

	avcodec_send_packet(codecContext, packet);

	ALint val;
	do {
		alGetSourcei(source, AL_SOURCE_STATE, &val);
	} while (val == AL_PLAYING);

	// Free OpenAL resources
	alDeleteSources(1, &source);
	alDeleteBuffers(NUM_BUFFERS, buffers);

	alcMakeContextCurrent(NULL);
	alcDestroyContext(ctx);
	alcCloseDevice(dev);

	av_packet_free(&packet);
	av_frame_free(&frame);
	avcodec_close(codecContext);
	avcodec_free_context(&codecContext);
	avformat_close_input(&formatContext);
	avformat_free_context(formatContext);

	return 0;
}