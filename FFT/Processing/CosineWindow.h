#pragma once
#include "WindowFunction.h"

class CosineWindow : public WindowFunction
{
public:
	CosineWindow()
	{

	}
	~CosineWindow()
	{

	}
protected:
	float value(int length, int index)
	{
		return (float)(cos(M_PI * index / ((int64_t)length - 1) - M_PI_2));
	}
};
