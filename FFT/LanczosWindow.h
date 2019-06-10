#pragma once
#include "WindowFunction.h"

class LanczosWindow : public WindowFunction
{
public:
	LanczosWindow()
	{

	}

protected:

	float value(int length, int index)
	{
		float x = 2 * index / (float)(length - 1) - 1;
		return (float)(sin(M_PI * x) / (M_PI * x));
	}
};