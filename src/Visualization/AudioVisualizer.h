#pragma once
#include "Utils/Resources.h"
#include "Utils/PacketQueue.h"

class AudioVisualizer
{
public:
	AudioVisualizer();
	virtual ~AudioVisualizer();
	virtual void init(AudioVisualizerInfo visualizerInfo) = 0;
	void run(PacketQueue<SampleContainer>* inputQueue)
	{
		static auto start = std::chrono::high_resolution_clock::now();
		while (!shouldClose())
		{
			//acquire ownership
			std::unique_ptr<SampleContainer> input = inputQueue->popPacket();
			if (input == nullptr)
				break;
			auto end = std::chrono::high_resolution_clock::now();
			int64_t delta = (end - start).count();

			if(input->timeStamp < delta)
			{
				continue;
			}
			//sleep until the timestamp of the input is reached
			std::this_thread::sleep_for(std::chrono::nanoseconds(input->timeStamp - delta));

			renderData(std::move(input));
		}
	}
	virtual void renderData(std::unique_ptr<SampleContainer> data) = 0;
protected:
	virtual bool shouldClose() = 0;
};