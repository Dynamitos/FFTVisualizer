#pragma once
#include "Resources.h"
#include "PacketQueue.h"

class AudioVisualizer
{
public:
	AudioVisualizer();
	virtual ~AudioVisualizer();
	virtual void init() = 0;
	void run(PacketQueue<SampleContainer>* inputQueue)
	{
		static auto start = std::chrono::high_resolution_clock::now();
		while (!inputQueue->isFinished())
		{
			auto end = std::chrono::high_resolution_clock::now();
			int64_t delta = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
			//acquire ownership
			std::unique_ptr<SampleContainer> input = inputQueue->popPacket();
			//sleep until the timestamp of the input is reached
			std::this_thread::sleep_for(std::chrono::nanoseconds(input->timeStamp - delta));

			renderData(std::move(input));
		}
	}
	virtual void renderData(std::unique_ptr<SampleContainer> data) = 0;
};