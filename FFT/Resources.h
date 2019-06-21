#pragma once
#include <AL/al.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdint.h>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
extern "C"
{
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
}

struct Line
{
	glm::vec2 start;
	glm::vec2 end;
	glm::vec4 lineColor;
};

namespace GL
{
	struct RawModel
	{
	public:
		GLuint vaoID;
		uint32_t vertexCount;
		const char* name;
		RawModel(GLuint vaoID, uint32_t vertexCount, const char* name)
			: vaoID(vaoID), vertexCount(vertexCount), name(name)
		{}
	};
};

struct ConverterInitInfo
{
	int inSampleRate;
	int outSampleRate;
	int inChannelCount;
	int outChannelCount;
	uint64_t inChannelLayout;
	uint64_t outChannelLayout;
	AVSampleFormat inSampleFormat;
	AVSampleFormat outSampleFormat;
};

struct Input
{
	static bool keys[GLFW_KEY_LAST];
};

struct PlayerInitInfo
{
	int numBuffers;
};

struct ProcessorInitInfo
{
	uint32_t sampleRate;
	uint32_t fftSize;
	uint32_t numBands;
	uint32_t tesselationLevel;
};

struct AudioVisualizerInfo
{
	const char* imageURL;
	glm::ivec2 screenDimensions;
	uint32_t numBands;
	uint32_t tesselationLevel;
	Line line;
	float intensityScale;
	float intensityOffset;
	bool scaling;
	bool vSync;
	const char* name;
	const char* overlayURL;
};

struct AudioSourceInfo
{
	const char* url;
};

struct AudioInputContainer
{
	AudioInputContainer()
		: samples{ nullptr }
		, allocatedChannels{ 0 }
		, allocatedData{ 0 }
		, numSamples{ 0 }
		, numChannels{ 0 }
		, dataSize{ 0 }
		, timeStamp{ 0 }
	{
	}
	AudioInputContainer(const AudioInputContainer& copy)
		: allocatedChannels{ copy.allocatedChannels }
		, allocatedData{ copy.allocatedData }
		, numSamples{ copy.numSamples }
		, numChannels{ copy.numChannels }
		, dataSize{ copy.dataSize }
		, timeStamp{ copy.timeStamp }
	{
		samples = new uint8_t * [copy.allocatedChannels];
		for (uint32_t i = 0; i < copy.allocatedChannels; ++i)
		{
			samples[i] = new uint8_t[copy.allocatedData];
			std::memcpy(samples[i], copy.samples[i], copy.allocatedData);
		}
	}
	AudioInputContainer(AudioInputContainer&& copy) noexcept
		: allocatedChannels{ std::move(copy.allocatedChannels) }
		, allocatedData{ std::move(copy.allocatedData) }
		, numSamples{ std::move(copy.numSamples) }
		, numChannels{ std::move(copy.numChannels) }
		, dataSize{ std::move(copy.dataSize) }
		, timeStamp{ std::move(copy.timeStamp) }
	{
		samples = copy.samples;
		copy.samples = nullptr;
		copy.allocatedChannels = 0;
		copy.allocatedData = 0;
		copy.numSamples = 0;
		copy.numSamples = 0;
		copy.dataSize = 0;
		copy.timeStamp = 0;
	}

	AudioInputContainer& operator=(const AudioInputContainer& other)
	{
		if (this != &other)
		{
			if (samples != nullptr)
			{
				for (uint32_t i = 0; i < allocatedChannels; ++i)
				{
					delete[] samples[i];
				}
				delete[] samples;
			}
			samples = new uint8_t * [other.allocatedChannels];
			for (uint32_t i = 0; i < other.allocatedChannels; ++i)
			{
				samples[i] = new uint8_t[other.allocatedData];
				std::memcpy(samples[i], other.samples[i], other.allocatedData);
			}

			allocatedChannels = other.allocatedChannels;
			allocatedData = other.allocatedData;
			numChannels = other.numChannels;
			numSamples = other.numSamples;
			dataSize = other.dataSize;
			timeStamp = other.timeStamp;

			return *this;
		}
	}

	AudioInputContainer& operator=(AudioInputContainer&& other) noexcept
	{
		if (this != &other)
		{
			//destroy current state
			if (samples != nullptr)
			{
				for (uint32_t i = 0; i < allocatedChannels; ++i)
				{
					delete[] samples[i];
				}
				delete[] samples;
			}

			samples = other.samples;
			other.samples = nullptr;
			allocatedChannels = std::move(other.allocatedChannels);
			allocatedData = std::move(other.allocatedData);
			numChannels = std::move(other.numChannels);
			numSamples = std::move(other.numSamples);
			dataSize = std::move(other.dataSize);
			timeStamp = std::move(other.timeStamp);

			return *this;
		}
	}
	~AudioInputContainer()
	{
		//free all channel buffers
		if (samples != nullptr)
		{
			//already allocated, free existing resources
			for (uint32_t i = 0; i < allocatedChannels; ++i)
			{
				//delete channel data
				delete[] samples[i];
			}
			delete samples;
		}
	}
	uint8_t** samples = nullptr;
	//used to track allocations of the buffer, may be bigger that usable portion
	uint32_t allocatedChannels = 0;
	uint32_t allocatedData = 0;
	uint32_t numSamples;
	uint32_t numChannels;
	uint32_t dataSize = 0;
	int64_t timeStamp;
};

struct SampleContainer
{
	SampleContainer()
		: convertedSamples{ nullptr }
		, dataSize{ 0 }
		, numSamples{ 0 }
		, timeStamp{ 0 }
	{

	}
	SampleContainer(const SampleContainer& copy)
		: dataSize{ copy.dataSize }
		, numSamples{ copy.numSamples }
		, timeStamp{ copy.timeStamp }
	{
		convertedSamples = new uint8_t[copy.dataSize];
		std::memcpy(convertedSamples, copy.convertedSamples, copy.dataSize);
	}
	SampleContainer(SampleContainer&& copy) noexcept
		: dataSize{ std::move(copy.dataSize) }
		, numSamples{ std::move(copy.numSamples) }
		, timeStamp{ std::move(copy.timeStamp) }
	{
		if (convertedSamples != nullptr)
		{
			delete[] convertedSamples;
		}
		convertedSamples = new uint8_t[dataSize];
		std::memcpy(convertedSamples, copy.convertedSamples, dataSize);
		delete[] copy.convertedSamples;
		copy.convertedSamples = nullptr;
	}
	SampleContainer& operator=(SampleContainer& other)
	{
		if (this != &other)
		{
			if (convertedSamples != nullptr)
			{
				delete[] convertedSamples;
			}
			
			convertedSamples = new uint8_t[other.dataSize];
			std::memcpy(convertedSamples, other.convertedSamples, other.dataSize);
			dataSize = other.dataSize;
			numSamples = other.numSamples;
			timeStamp = other.timeStamp;

			return *this;
		}
	}

	SampleContainer& operator=(SampleContainer&& other) noexcept
	{
		if (this != &other)
		{
			if (convertedSamples != nullptr)
			{
				delete[] convertedSamples;
			}
			dataSize = std::move(other.dataSize);
			numSamples = std::move(other.numSamples);
			timeStamp = std::move(other.timeStamp);
			convertedSamples = new uint8_t[dataSize];
			std::memcpy(convertedSamples, other.convertedSamples, dataSize);
			delete[] other.convertedSamples;
			other.convertedSamples = nullptr;

			return *this;
		}
	}
	~SampleContainer()
	{
		if (convertedSamples != nullptr)
		{
			delete[] convertedSamples;
		}
	}
	uint8_t* convertedSamples = nullptr;
	uint32_t dataSize = 0;
	uint32_t numSamples;
	int64_t timeStamp;
};
