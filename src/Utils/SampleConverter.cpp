#include "SampleConverter.h"
#include <stdexcept>
#include <iostream>
extern "C"
{
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
}

SampleConverter::SampleConverter(ConverterInitInfo converterInfo)
    : initInfo{ converterInfo }
{
    swrContext = swr_alloc();

    av_opt_set_int(swrContext, "in_sample_rate", converterInfo.inSampleRate, 0);
    av_opt_set_int(swrContext, "out_sample_rate", converterInfo.outSampleRate, 0);

    av_opt_set_int(swrContext, "in_channel_count", converterInfo.inChannelCount, 0);
    av_opt_set_int(swrContext, "out_channel_count", converterInfo.outChannelCount, 0);

    av_opt_set_channel_layout(swrContext, "in_channel_layout", converterInfo.inChannelLayout, 0);
    av_opt_set_channel_layout(swrContext, "out_channel_layout", converterInfo.outChannelLayout, 0);

    av_opt_set_sample_fmt(swrContext, "in_sample_fmt", converterInfo.inSampleFormat, 0);
    av_opt_set_sample_fmt(swrContext, "out_sample_fmt", converterInfo.outSampleFormat, 0);

    int ret = swr_init(swrContext);
    if (ret < 0)
    {
        printf("Exiting: %d", ret);
    }
}

SampleConverter::~SampleConverter()
{
    swr_free(&swrContext);
}

void SampleConverter::run(PacketQueue<AudioInputContainer>* inputQueue, PacketQueue<SampleContainer>* outputQueue)
{
    while (std::unique_ptr<AudioInputContainer> input = inputQueue->popPacket())
    {
        if (input == nullptr)
            break;
        std::unique_ptr<SampleContainer> output = convertFrame(std::move(input));
        outputQueue->addToQueue(std::move(output));
    }
    outputQueue->signalFinished();
    printf("Converting done\n");
}

std::unique_ptr<SampleContainer> SampleConverter::convertFrame(std::unique_ptr<AudioInputContainer> container)
{
    std::unique_ptr<SampleContainer> result = std::make_unique<SampleContainer>();
    uint64_t maxBytes = av_samples_get_buffer_size(nullptr, initInfo.outChannelCount, container->numSamples, initInfo.outSampleFormat, 1);
    
    if (result->dataSize < maxBytes) {
        delete[] result->convertedSamples;
        result->convertedSamples = nullptr;
    }
    if (result->convertedSamples == nullptr)
    {
        result->convertedSamples = new uint8_t[maxBytes];
        result->dataSize = static_cast<uint32_t>(maxBytes);
    }
    uint8_t* outputBuffer = result->convertedSamples;

    //std::cout << "Output buffer location: " << &result->convertedSamples << std::endl;
    int ret = swr_convert(swrContext, &outputBuffer, container->numSamples, (const uint8_t * *)container->samples, container->numSamples);
    if (ret < 0) {
        throw new std::logic_error("Error while converting\n");
    }

    result->timeStamp = container->timeStamp;
    result->numSamples = container->numSamples;
    return result;
}
