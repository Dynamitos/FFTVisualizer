#pragma once
#include "WindowFunction.h"

class HannWindow : public WindowFunction
{
public:
	HannWindow()
	{
	}
	~HannWindow()
	{

	}
protected:
	float value(int length, int index)
	{
		return 0.5f * (1.0f - (float)cos(TWO_PI * index / ((double)length - 1.0f)));
	}
};