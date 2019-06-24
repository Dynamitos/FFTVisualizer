#pragma once
#include "Utils/Resources.h"
#include "Utils/PacketQueue.h"
/**
* This inteface defines the capabilities of an Audio Input. This can be implemented via reading from a file, streaming from a server etc.
*/
class AudioInput
{
public:
	AudioInput()
	{

	}
	virtual ~AudioInput()
	{

	}

	void registerPacketQueue(PacketQueue<AudioInputContainer>* newQueue)
	{
		queues.push_back(newQueue);
	}

	void run()
	{
		std::unique_ptr<AudioInputContainer> samples = readSamples();
		while (samples != nullptr)
		{
			for (auto queue = queues.begin(); queue != queues.end(); ++queue)
			{
				(*queue)->addToQueue(std::make_unique<AudioInputContainer>(*samples));
			}
			samples = readSamples();
		}
		for (auto queue = queues.begin(); queue != queues.end(); ++queue)
		{
			(*queue)->signalFinished();
		}
	}

	/**
	* This method can be used to initialize the AudioSource, for example open a filestream, or requesting an ftp connection to an URL
	* Additional information about the input like format and frequency are to be provided here
	*/
	virtual void setStreamSource(AudioSourceInfo& info) = 0;

	/**
	* This method is used to read one set of samples from the current audio input. The samples are currently to be passed as floats, however there will be changes in the future
	*/
	virtual std::unique_ptr<AudioInputContainer> readSamples() = 0;

	/**
	* Returns the sample rate of the input stream(before converting to the player)
	*/
	virtual uint32_t getSampleRate() = 0;

	/**
	* Fills the attributes of the given ConverterInitInfo with the values from the input.
	* Defaults the output attributes to the same as the input so that the player only has to change the relevant fields.
	*/
	virtual void fillConverterInfo(ConverterInitInfo& converterInfo) = 0;
private:
	std::vector<PacketQueue<AudioInputContainer>*> queues;
};