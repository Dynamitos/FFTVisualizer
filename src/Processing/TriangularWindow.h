#pragma once
#include "WindowFunction.h"

class TriangularWindow : public WindowFunction
{
public:
	TriangularWindow()
	{

	}
	~TriangularWindow() 
	{

	}

protected:
	float value(int length, int index)
	{
		return 2.f / length * (length / 2.f - abs(index - (length - 1) / 2.f));
	}
};
