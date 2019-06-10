#pragma once
#include "Resources.h"
class AudioProcessor
{
public:
	AudioProcessor()
	{

	}
	virtual ~AudioProcessor()
	{

	}
	virtual void init() = 0;

	virtual void finalizeConverterInfo(ConverterInitInfo& initInfo) = 0;

	virtual void forward(const ConvertedSampleContainer& container) = 0;

};