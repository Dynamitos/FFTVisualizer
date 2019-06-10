#pragma once
#include "WindowFunction.h"
class RectangularWindow : public WindowFunction
{
public:
	RectangularWindow()
	{

	}
	~RectangularWindow()
	{

	}
protected:
	float value(int length, int index)
	{
		return 1.f;
	}
};