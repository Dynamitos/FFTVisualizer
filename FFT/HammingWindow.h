#pragma once
#include "WindowFunction.h"

class HammingWindow : public WindowFunction
{
public:
	HammingWindow()
	{

	}
	~HammingWindow()
	{

	}
protected:
	float value(int length, int index)
	{
		return 0.54f - 0.46f * (float)cos(TWO_PI * index / ((double)length - 1));
	}
};