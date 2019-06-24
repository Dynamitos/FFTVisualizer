#pragma once
#include "Utils/Resources.h"
#include "Utils/PacketQueue.h"
class AudioProcessor
{
public:
	AudioProcessor()
		: outputQueue{ nullptr }
	{

	}
	virtual ~AudioProcessor()
	{

	}
	void run(PacketQueue<SampleContainer>* inputQueue, PacketQueue<SampleContainer>* outputQueue)
	{
		this->outputQueue = outputQueue;
		while (!inputQueue->isFinished())
		{
			//dont add anything to queue since each queue is responsible for their own ouput frequency
			forward(inputQueue->popPacket());
		}
		outputQueue->signalFinished();
	}

	virtual void init(ProcessorInitInfo processorInfo) = 0;

	virtual void finalizeConverterInfo(ConverterInitInfo& initInfo) = 0;

	virtual void forward(std::unique_ptr<SampleContainer> container) = 0;
protected:
	//Since input and output are not in sync for processors, each processor is responsible to use the queue
	PacketQueue<SampleContainer>* outputQueue;
};