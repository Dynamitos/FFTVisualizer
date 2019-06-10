#pragma once
#include "WindowFunction.h"

class BartlettHannWindow : public WindowFunction
{
public:
	BartlettHannWindow()
	{
	}

protected:
	float value(int length, int index)
	{
		return (float)(0.62f - 0.48f * abs(index / (length - 1) - 0.5f) - 0.38 * cos(TWO_PI * index / ((int64_t)length - 1)));
	}
};
